// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCode.h"
#include "VRHand.h"
#include "IPickupable.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"

// Sets default values
AVRHand::AVRHand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hand = EControllerHand::Right;
	Grip = EGripState::Open;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>( TEXT( "MotionController" ) );
	//MotionController->SetupAttachment( RootComponent );
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

	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportCylinder" ) );

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
		//    SCLARK39 NOTE:
		//			This is bad code. Data should not become invalidated in this way. When another 
		//			hand/event occurs, it should remove the reference as the hand stops holding it so
		//			it can properly respond to new input.
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

}

