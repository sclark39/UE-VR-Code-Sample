// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCode.h"
#include "VRHand.h"
#include "IPickupable.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/Components/SplineMeshComponent.h"

// Sets default values
AVRHand::AVRHand() :
	Extents( 500, 500, 500 )
{


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hand = EControllerHand::Right;
	Grip = EGripState::Open;

	Scene = CreateDefaultSubobject<USceneComponent>( TEXT( "Scene" ) );

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>( TEXT( "MotionController" ) );
	MotionController->SetupAttachment( Scene );
	MotionController->Hand = Hand;

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "HandMesh" ) );
	HandMesh->SetupAttachment( MotionController );

	GrabSphere = CreateDefaultSubobject<USphereComponent>( TEXT( "GrabSphere" ) );
	GrabSphere->SetupAttachment( HandMesh ); 
	GrabSphere->InitSphereRadius( 10.0f );
	GrabSphere->OnComponentBeginOverlap.AddDynamic( this, &AVRHand::OnComponentBeginOverlap );

	ArcDirection = CreateDefaultSubobject<UArrowComponent>( TEXT( "ArcDirection" ) );
	ArcDirection->SetupAttachment( HandMesh );

	ArcSpline = CreateDefaultSubobject<USplineComponent>( TEXT( "ArcSpline" ) );
	ArcSpline->SetupAttachment( HandMesh );

	ArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "ArcEndPoint" ) );
	ArcEndPoint->SetupAttachment( Scene );
	ArcEndPoint->SetVisibility( false );

	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportCylinder" ) );
	TeleportCylinder->SetupAttachment( Scene );

	TeleportRing = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportRing" ) );
	TeleportRing->SetupAttachment( TeleportCylinder );

	TeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportArrow" ) );
	TeleportArrow->SetupAttachment( TeleportCylinder );
	
}

void AVRHand::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	if (Hand == EControllerHand::Left)
	{
		// Reflect hand mesh
		HandMesh->SetWorldScale3D( FVector( 1, 1, -1 ) );
	}

}

void AVRHand::RumbleController_Implementation( float intensity )
{
	bool isLeft = Hand == EControllerHand::Left;

	FLatentActionInfo actionInfo;
	actionInfo.CallbackTarget = this;
	APlayerController *playerController = GetWorld()->GetFirstPlayerController();

	playerController->PlayDynamicForceFeedback( intensity, 
												0.04f,
												isLeft, isLeft,
												!isLeft, !isLeft,
												EDynamicForceFeedbackAction::Start,
												actionInfo );
}

void AVRHand::OnComponentBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	Grip = EGripState::CanGrab;
	if ( ( OtherComp != nullptr ) && ( OtherComp != GrabSphere ) )
	{
		UStaticMeshComponent *mesh = Cast<UStaticMeshComponent>( OtherComp );
		if ( mesh && mesh->IsSimulatingPhysics() )
		{
			this->RumbleController( 0.8 );
		}
	}
}

// Called when the game starts or when spawned
void AVRHand::BeginPlay()
{
	MotionController->Hand = Hand;

	Super::BeginPlay();

	// HACK FIX: ISSUE UE-41708
	//   We need to wait until play begins to set the blueprint for the child actor so we 
	//   also need to do the flip transform in BeginPlay, since OnConstruction is too early.
	if ( Hand == EControllerHand::Left )
	{
		// Reflect hand mesh
		HandMesh->SetWorldScale3D( FVector( 1, 1, -1 ) );
	}	

	// Hide until activation (but wait for BeginPlay so it is shown in editor)
	TeleportCylinder->SetVisibility( false, true );
}

AActor* AVRHand::GetActorNearHand()
{
	TArray<AActor*> overlappingActors;

	GrabSphere->GetOverlappingActors( overlappingActors );
	FVector handLocation = GrabSphere->GetComponentLocation();

	AActor* nearest = nullptr;
	float mindist = 99999999999;

	// Find closest overlaping actor
	for ( AActor *actor : overlappingActors )
	{
		bool isPickupable = actor->GetClass()->ImplementsInterface( UPickupable::StaticClass() );
		if ( isPickupable )
		{

			float dist = ( actor->GetActorLocation() - handLocation ).SizeSquared();
			if ( dist < mindist )
			{
				mindist = dist;
				nearest = actor;
			}
		}
	}

	// 	if ( GEngine && Hand == EControllerHand::Right )
// 		GEngine->AddOnScreenDebugMessage( -1, 0.16f, FColor::Red,
// 			FString::Printf( TEXT( "Actors near right hand %d, found pickupable: %d, %s" ), 
// 			overlappingActors.Num(), 
// 			count,
// 			nearest ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ) );

	return nearest;
}

void AVRHand::UpdateAnimationGripState()
{
	// Default to Open
	Grip = EGripState::Open;

	if ( AttachedActor )
	{
		// If holding an object, always keep fist closed
		Grip = EGripState::Grab;
	}
	else
	{
		// React to player input
		if ( WantsToGrip )
			Grip = EGripState::Grab;

		// If not holding something, the hand should open or close 
		// slightly when passing over an interactable object
		AActor *actor = GetActorNearHand();
		if ( actor )
			Grip = EGripState::CanGrab;
	}

	// Only let hand collide with environment while gripping
	if ( Grip == EGripState::Grab )
		HandMesh->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
	else
		HandMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision );
}

void AVRHand::GrabActor_Implementation()
{
	WantsToGrip = true;

	AActor *actor = GetActorNearHand();
	if ( actor && actor->IsValidLowLevel() )
	{
		AttachedActor = actor;
		IPickupable::Execute_Pickup( actor, MotionController );
		RumbleController( 0.7 );
	}

}

void AVRHand::ReleaseActor_Implementation()
{
	WantsToGrip = false;

	AActor *actor = AttachedActor;
	if ( actor && actor->IsValidLowLevel() )
	{
		// Make sure this hand is still holding the Actor (May have been taken by another hand / event)
		if ( MotionController == actor->GetRootComponent()->GetAttachParent() )
		{
			IPickupable::Execute_Drop( actor );
			RumbleController( 0.2 );
		}
	}

	AttachedActor = nullptr;
}

// Called every frame
void AVRHand::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	UpdateAnimationGripState();

	if ( IsTeleporterActive )
	{
		TArray<FVector> TracePoints;
		FVector NavLocation;
		FVector HitLocation;
		bool IsValidTeleportDestination = TraceTeleportDestination( TracePoints, NavLocation, HitLocation );

		TeleportCylinder->SetVisibility( IsValidTeleportDestination, true );

		if ( IsValidTeleportDestination )
		{
			// Find Floor at Teleport Location and Move Cylinder
			FHitResult OutHit;
			{
				FVector EndPos = NavLocation + FVector( 0, 0, -200 );
				FCollisionQueryParams CollisionQueryParams( FName( TEXT( "TeleporterDrop" ) ), false, this );
				GetWorld()->LineTraceSingleByChannel( OutHit, NavLocation, EndPos, ECC_WorldStatic, CollisionQueryParams );
			}

			FVector TeleportCylinderLocation;
			if ( OutHit.bBlockingHit )
				TeleportCylinderLocation = OutHit.ImpactPoint;
			else
				TeleportCylinderLocation = NavLocation;

			TeleportCylinder->SetWorldLocation( TeleportCylinderLocation, false, nullptr, ETeleportType::TeleportPhysics );

			ArcEndPoint->SetVisibility( true );
			ArcEndPoint->SetWorldLocation( HitLocation, false, nullptr, ETeleportType::TeleportPhysics );

			// Rotate Arrow
			FRotator ArrowRotator = TeleportRotator;

 			IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
			if ( hmd )
			{
				FRotator DeviceRotation;
				FVector DevicePosition;
				UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition( DeviceRotation, DevicePosition );

				DeviceRotation.Pitch = 0;
				DeviceRotation.Roll = 0;

				ArrowRotator = UKismetMathLibrary::ComposeRotators( TeleportRotator, DeviceRotation );
			}

			TeleportArrow->SetWorldRotation( ArrowRotator );

			
			// Destroy old Spline
			for ( USplineMeshComponent *SplineMesh : SplineMeshes )
				SplineMesh->DestroyComponent();
			SplineMeshes.Reset();
			ArcSpline->ClearSplinePoints();

			// Make Spline....
			if ( TracePoints.Num() > 0 )
			{
				for ( FVector TracePoint : TracePoints )
				{
					ArcSpline->AddSplinePoint( TracePoint, ESplineCoordinateSpace::Local, true );
				}
				ArcSpline->SetSplinePointType( TracePoints.Num() - 1, ESplinePointType::CurveClamped, true );

				for ( int i = 0; i < TracePoints.Num() - 2; i++ )
				{
					FVector StartPos = TracePoints[i];
					FVector StartTangent = ArcSpline->GetTangentAtSplinePoint( i, ESplineCoordinateSpace::Local );

					FVector EndPos = TracePoints[i + 1];
					FVector EndTangent = ArcSpline->GetTangentAtSplinePoint( i + 1, ESplineCoordinateSpace::Local );

					USplineMeshComponent *SplineMeshComponent = NewObject<USplineMeshComponent>( this, USplineMeshComponent::StaticClass() );
					SplineMeshComponent->SetStaticMesh( BeamMesh );
					SplineMeshComponent->SetMaterial( 0, BeamMaterial );
					SplineMeshComponent->SetStartScale( FVector2D( 4, 4 ) );
					SplineMeshComponent->SetEndScale( FVector2D( 4, 4 ) );
					SplineMeshComponent->SetBoundaryMax( 1 );

					SplineMeshComponent->SetStartAndEnd( StartPos, StartTangent, EndPos, EndTangent );


					SplineMeshes.Push( SplineMeshComponent );
				}
				RegisterAllComponents();


			}

			
		}

		// If it changed, rumble.
		if ( LastIsValidTeleportDestination != IsValidTeleportDestination )
			RumbleController( 0.3 );
		LastIsValidTeleportDestination = IsValidTeleportDestination;
	}
	
}

void AVRHand::ActivateTeleporter()
{
	IsTeleporterActive = true;
}


void AVRHand::DisableTeleporter()
{
	IsTeleporterActive = false;
	TeleportCylinder->SetVisibility( false, true );
	ArcEndPoint->SetVisibility( false );
	// TODO: Roomscale Mesh
}

bool AVRHand::TraceTeleportDestination( TArray<FVector> &TracePoints, FVector &NavMeshLocation, FVector &TraceLocation )
{

	FVector StartPos = ArcDirection->GetComponentLocation();
	FVector LaunchVelocity = ArcDirection->GetForwardVector();

	LaunchVelocity *= TeleportLaunchVelocity;

	// Predict Projectile Path
	FHitResult OutHit;
	TArray<FVector> OutPathPositions;
	FVector OutLastTraceDestination;
	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Push( 
		UEngineTypes::ConvertToObjectType( ECC_WorldStatic )
	);
	const bool DidPredictPath = UGameplayStatics::PredictProjectilePath( GetWorld(), OutHit, OutPathPositions, OutLastTraceDestination,
		StartPos, LaunchVelocity, true, 0, ObjectTypes, false, ActorsToIgnore,
		EDrawDebugTrace::Type::None, 0 );
	if ( !DidPredictPath )
		return false;

	// Getting Projected Endpoint
	FVector PointToProject = OutHit.Location;
	FNavLocation ProjectedHitLocation; 
	UNavigationSystem *NavSystem = GetWorld()->GetNavigationSystem();
	const bool DidProjectToNav = NavSystem->ProjectPointToNavigation( PointToProject, ProjectedHitLocation, Extents );
	if ( !DidProjectToNav )
		return false;

	// Outputs...
	TracePoints = OutPathPositions;
	TraceLocation = OutHit.Location;
	NavMeshLocation = ProjectedHitLocation.Location;	
	return true;
}


void AVRHand::GetTeleportDestination( FVector &OutPosition, FRotator &OutRotator )
{
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	FVector DevicePosition(ForceInitToZero);
	if ( hmd )
	{
		FRotator DeviceRotation;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition( DeviceRotation, DevicePosition );
		DevicePosition.Z = 0; // Ignore relative height difference
	}

	DevicePosition = TeleportRotator.RotateVector( DevicePosition );

	// Substract HMD origin (Camera) to get correct Pawn destination for teleportation.
	OutPosition = TeleportCylinder->GetComponentLocation() - DevicePosition;
	OutRotator = TeleportRotator;
}