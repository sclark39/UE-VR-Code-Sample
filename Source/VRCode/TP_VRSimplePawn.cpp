/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/


#include "VRCode.h"
#include "GamepadVRPawn.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/HeadMountedDisplayFunctionLibrary.h"


// Sets default values
AGamepadVRPawn::AGamepadVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>( TEXT( "Scene" ) );

	VROrigin = CreateDefaultSubobject<USceneComponent>( TEXT( "VROrigin" ) );
	VROrigin->SetupAttachment( Scene );

	Camera = CreateDefaultSubobject<UCameraComponent>( TEXT( "Camera" ) );
	Camera->SetupAttachment( VROrigin );

	TraceDirection = CreateDefaultSubobject<UArrowComponent>( TEXT( "TraceDirection" ) );
	TraceDirection->SetupAttachment( Camera );
	
	TeleportPin = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportPin" ) );
	TeleportPin->SetupAttachment( Scene );

	TeleportRing = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportRing" ) );
	TeleportRing->SetupAttachment( TeleportPin );

	TeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "TeleportArrow" ) );
	TeleportArrow->SetupAttachment( TeleportPin );
}

// Called when the game starts or when spawned
void AGamepadVRPawn::BeginPlay()
{
	Super::BeginPlay();

	// Setup Player Height for Various Platforms
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	if ( hmd )
	{
		EHMDDeviceType::Type DeviceType = hmd->GetHMDDeviceType();

		if ( DeviceType == EHMDDeviceType::DT_SteamVR || DeviceType == EHMDDeviceType::DT_OculusRift )
		{
			hmd->SetTrackingOrigin( EHMDTrackingOrigin::Floor );
		}
		else
		{
			// PSVR
			hmd->SetTrackingOrigin( EHMDTrackingOrigin::Eye );

			// Set Height Offset for Tracking			
			VROrigin->AddLocalOffset( FVector( 0, 0, DefaultPlayerHeight ) );
		}
	}

	// Create MID to give activation feedback during teleportation
	RingGlowMaterialInstance = TeleportPin->CreateDynamicMaterialInstance( 0 );
}


void AGamepadVRPawn::FinishTeleport()
{
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	if ( hmd )
	{
		FRotator DeviceRotation;
		FVector DevicePosition;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition( DeviceRotation, DevicePosition );
		DevicePosition.Z = 0;

		const FRotator TeleportRotator = PinnedRotation;
		const FVector TeleportLocation = PinnedLocation - TeleportRotator.RotateVector( DevicePosition );

		TeleportTo( TeleportLocation, TeleportRotator );


		// Fade back in
		APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager( GetWorld(), 0 );
		PlayerCamera->StartCameraFade( 1, 0, FadeInDuration, TeleportFadeColor, false, true );
	}
}

void AGamepadVRPawn::ExecuteTeleport()
{
	if ( !LocationPinned )
		return;

	LocationPinned = false;

	if ( !LocationFound )
		return;

	APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager( GetWorld(), 0 );
	PlayerCamera->StartCameraFade( 0, 1, FadeOutDuration, TeleportFadeColor, false, true );

	// Wait for Fade to complete before continuing the teleport
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer( TimerHandle, this, &AGamepadVRPawn::FinishTeleport, FadeOutDuration, false );
}


void AGamepadVRPawn::ActivateTeleport()
{
	if ( CurrentLocationValid )
	{
		LocationPinned = true;
		PinnedLocation = CurrentLookAtLocation;
	}

	LocationFound = CurrentLocationValid;
}

void AGamepadVRPawn::UpdateTeleportDirection( const FVector2D &StickInput )
{
	if ( bUseGamepad )
	{
		// Get Thumbstick Facing Direction
		const FVector ActorDirection = PinnedLocation - Camera->GetComponentLocation();
		FRotator ActorRotator = ActorDirection.ToOrientationRotator();
		ActorRotator.Roll = 0;
		ActorRotator.Pitch = 0;

		FVector StickDirection( StickInput, 0 );
		StickDirection.Normalize();
		StickDirection *= 400.0;

		const FVector TeleportDirection = ActorRotator.RotateVector( StickDirection );

		PinnedRotation = TeleportDirection.ToOrientationRotator();

	}
	else
	{
		const FVector LookAtDirection = ( CurrentLookAtLocation - PinnedLocation ).GetSafeNormal();
		FRotator LookAtRotator = LookAtDirection.ToOrientationRotator();
		LookAtRotator.Pitch = 0;
		LookAtRotator.Roll = 0;

		PinnedRotation = LookAtRotator;
	}
}

// Called every frame
void AGamepadVRPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CurrentLocationValid = GetTeleportDirection( CurrentLookAtLocation );

	const FVector2D StickInput(
		InputComponent ? InputComponent->GetAxisValue( TEXT( "TeleportDirectionUp" ) ) : 0,
		InputComponent ? InputComponent->GetAxisValue( TEXT( "TeleportDirectionRight" ) ) : 0
	);

	if ( LocationPinned )
	{
		UpdateTeleportDirection( StickInput );
	}
	else
	{
		// Find and show a potential spot to teleport to
		TeleportPin->SetWorldLocation( CurrentLookAtLocation, false, nullptr, ETeleportType::TeleportPhysics );
		TeleportPin->SetVisibility( CurrentLocationValid, true );
	}

	
	// Update Teleport Meshes Position and Orientations
	bool bShouldUpdateFacing = false;
	if ( bUseGamepad )
	{
		bShouldUpdateFacing = StickInput.SizeSquared() > ( ThumbDeadzone * ThumbDeadzone );
	}
	else
	{
		const FVector LookAtDirection = ( CurrentLookAtLocation - PinnedLocation );
		bShouldUpdateFacing = LookAtDirection.SizeSquared() > RotationLengthThreshold * RotationLengthThreshold;
	}

	const bool bArrowIsVisible = bShouldUpdateFacing && LocationPinned;

	TeleportArrow->SetVisibility( bArrowIsVisible, false );
	RingGlowMaterialInstance->SetScalarParameterValue( TEXT( "HeightScale" ), LocationPinned ? 1.0 : 0.35 );

	if ( bArrowIsVisible )
	{
		if ( bUseGamepad )
		{
			IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
			if ( hmd )
			{
				FRotator DeviceRotation;
				FVector DevicePosition;
				UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition( DeviceRotation, DevicePosition );

				DeviceRotation.Pitch = 0;
				DeviceRotation.Roll = 0;

				const FRotator ArrowRotator = UKismetMathLibrary::ComposeRotators( PinnedRotation, DeviceRotation );
				TeleportArrow->SetWorldRotation( ArrowRotator );
			}
		}
		else
		{
			TeleportArrow->SetWorldRotation( PinnedRotation );
		}
	}

}

// Called to bind functionality to input
void AGamepadVRPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

	PlayerInputComponent->BindAction( TEXT( "HMDTeleport" ), IE_Pressed, this, &AGamepadVRPawn::ActivateTeleport );
	PlayerInputComponent->BindAction( TEXT( "HMDTeleport" ), IE_Released, this, &AGamepadVRPawn::ExecuteTeleport );

	PlayerInputComponent->BindAxis( TEXT( "TeleportDirectionUp" ) );
	PlayerInputComponent->BindAxis( TEXT( "TeleportDirectionRight" ) );

}


bool AGamepadVRPawn::GetTeleportDirection( FVector &OutLocation )
{
	FVector StartPos = TraceDirection->GetComponentLocation();
	FVector LaunchVelocity = TraceDirection->GetForwardVector() * 10000.0;

	// Predict Projectile Path
	FPredictProjectilePathParams PredictParams( 0.0f, StartPos, LaunchVelocity, 2.0f, UEngineTypes::ConvertToObjectType( ECC_WorldStatic ) );
	FPredictProjectilePathResult PredictResult;
	const bool DidPredictPath = UGameplayStatics::PredictProjectilePath( GetWorld(), PredictParams, PredictResult );
	if ( !DidPredictPath )
		return false;

	// Getting Projected Endpoint
	FVector PointToProject = PredictResult.HitResult.Location;
	FNavLocation ProjectedHitLocation;
	UNavigationSystem *NavSystem = GetWorld()->GetNavigationSystem();
	const bool DidProjectToNav = NavSystem->ProjectPointToNavigation( PointToProject, ProjectedHitLocation, Extents );
	if ( !DidProjectToNav )
		return false;

	
	OutLocation = ProjectedHitLocation.Location;
	return true;
}