// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCode.h"
#include "VRPawn.h"
#include "VRHand.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);

	LeftHand = CreateDefaultSubobject<UChildActorComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(VROrigin);

	RightHand = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(VROrigin);

	DeviceType = EHMDDeviceType::DT_ES2GenericStereoMesh;
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	// Setup Player Height for Various Platforms
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	if ( hmd )
	{
		DeviceType = hmd->GetHMDDeviceType();

		if ( DeviceType == EHMDDeviceType::DT_SteamVR || DeviceType == EHMDDeviceType::DT_OculusRift )
		{
			hmd->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
		}
		else
		{
			// PSVR
			hmd->SetTrackingOrigin(EHMDTrackingOrigin::Eye);

			// Set Height Offset for Tracking
			FVector defaultHeightOffset(0, 0, 180);
			VROrigin->AddLocalOffset(defaultHeightOffset);
		}
	}

}

void AVRPawn::FinishTeleport( AVRHand *Current, const FVector &TeleportPosition, const FRotator &TeleportRotator )
{
	const float kFadeInDuration = 0.1;
	const FLinearColor kTeleportFadeColor = FLinearColor::Black;

	Current->DisableTeleporter();

	// Move the player
	TeleportTo( TeleportPosition, TeleportRotator, false, false );

	// Fade back in
	APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager( GetWorld(), 0 );
	PlayerCamera->StartCameraFade( 1, 0, kFadeInDuration, kTeleportFadeColor, false, true );

	// All done.
	IsTeleporting = false;
}

void AVRPawn::ExecuteTeleport( AVRHand *Current )
{
	const float kFadeOutDuration = 0.1;
	const FLinearColor kTeleportFadeColor = FLinearColor::Black;

	if ( IsTeleporting )
		return;

	// 	if ( !Current->HasValidTeleportLocation )
	// 	{
	// 		Current->DisableTeleporter();
	// 		return;
	// 	}

	FVector TeleportPosition;
	FRotator TeleportRotator;

	Current->GetTeleportDestination( TeleportPosition, TeleportRotator );

	// We're doing this!
	IsTeleporting = true;

	// Fade out screen
	APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager( GetWorld(), 0 );
	PlayerCamera->StartCameraFade( 0, 1, kFadeOutDuration, kTeleportFadeColor, false, true );
	
	// Wait for Fade to complete before continuing the teleport
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction( this, FName( TEXT( "FinishTeleport" ) ), Current, TeleportPosition, TeleportRotator );
	GetWorldTimerManager().SetTimer( TimerHandle, TimerDelegate, kFadeOutDuration, false );
}

void AVRPawn::HandleStickInputStyleTeleportActivation( FVector2D AxisInput, AVRHand *Current, AVRHand *Other )
{
	const float ThumbDeadzone = 0.7;
	const float ThumbDeadzoneSq = ThumbDeadzone * ThumbDeadzone;

	if ( AxisInput.SizeSquared() > ThumbDeadzoneSq )
	{
		if ( Current )
			Current->ActivateTeleporter();
		if ( Other )
			Other->DisableTeleporter();
	}
	else 
	{
		if ( Current && Current->IsTeleporterActive )		
			ExecuteTeleport( Current );
	}
}

bool AVRPawn::GetRotationFromInput( FVector2D AxisInput, FRotator &OrientRotator )
{
	const float ThumbDeadzone = 0.7;

	FRotator ActorRotator = GetActorRotation();
	ActorRotator.Roll = 0;
	ActorRotator.Pitch = 0;


	const float ThumbDeadzoneSq = ThumbDeadzone * ThumbDeadzone;
	if ( AxisInput.SizeSquared() > ThumbDeadzoneSq )
	{
		FVector InputVector( AxisInput, 0 ); 
		InputVector.Normalize();
		InputVector = ActorRotator.RotateVector( InputVector );

		OrientRotator = InputVector.ToOrientationRotator();
		return true;
	}

	OrientRotator = ActorRotator;
	return false;
}

// Called every frame
void AVRPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ( InputComponent )
	{
		AVRHand *Left = Cast<AVRHand>( LeftHand->GetChildActor() );
		FVector2D ThumbLeft(
			InputComponent->GetAxisValue( TEXT( "ThumbLeft_Fwd" ) ),
			InputComponent->GetAxisValue( TEXT( "ThumbLeft_Side" ) )
		);

		AVRHand *Right = Cast<AVRHand>( RightHand->GetChildActor() );
		FVector2D ThumbRight(
			InputComponent->GetAxisValue( TEXT( "ThumbRight_Fwd" ) ),
			InputComponent->GetAxisValue( TEXT( "ThumbRight_Side" ) )
		);

		// Robo Rally style Teleport Activation
		if ( DeviceType == EHMDDeviceType::DT_OculusRift )
		{			
			HandleStickInputStyleTeleportActivation( ThumbLeft, Left, Right );
			HandleStickInputStyleTeleportActivation( ThumbRight, Right, Left );
		}

		// Teleport Rotation
		FRotator OrientRotator;

		if ( Left->IsTeleporterActive )
		{
			// Robo Rally style, don't change teleport rotator when releasing the stick
			if ( GetRotationFromInput( ThumbLeft, OrientRotator ) )
				Left->TeleportRotator = OrientRotator; 
		}

		if ( Right->IsTeleporterActive )
		{
			// Robo Rally style, don't change teleport rotator when releasing the stick
			if ( GetRotationFromInput( ThumbRight, OrientRotator ) )
				Right->TeleportRotator = OrientRotator;
		}

	}

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );
	
	PlayerInputComponent->BindAction( "GripLeft", IE_Pressed, this, &AVRPawn::GripLeft );
	PlayerInputComponent->BindAction( "GripLeft", IE_Released, this, &AVRPawn::StopGripLeft );

	PlayerInputComponent->BindAction( "GripRight", IE_Pressed, this, &AVRPawn::GripRight );
	PlayerInputComponent->BindAction( "GripRight", IE_Released, this, &AVRPawn::StopGripRight );

	PlayerInputComponent->BindAxis( TEXT( "ThumbLeft_Fwd" ) );
	PlayerInputComponent->BindAxis( TEXT( "ThumbRight_Fwd" ) );

	PlayerInputComponent->BindAxis( TEXT( "ThumbLeft_Side" ) );
	PlayerInputComponent->BindAxis( TEXT( "ThumbRight_Side" ) );
}

void AVRPawn::UpdateGrip( UChildActorComponent *hand, bool pressed )
{
	AVRHand *vrHand = Cast<AVRHand>( hand->GetChildActor() );
	if ( vrHand )
	{
		if ( pressed )
			vrHand->GrabActor();
		else // released
			vrHand->ReleaseActor();
	}
}
void AVRPawn::GripLeft()		{ UpdateGrip( LeftHand, true ); }
void AVRPawn::StopGripLeft()	{ UpdateGrip( LeftHand, false ); }
void AVRPawn::GripRight()		{ UpdateGrip( RightHand, true ); }
void AVRPawn::StopGripRight()	{ UpdateGrip( RightHand, false ); }