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

void AVRPawn::ExecuteTeleport_Implementation()
{

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
			ExecuteTeleport();
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
		// Needed to alternate the inputs because the Y is forward, on the thumb stick, and X is forward with rotators...
		// I think this math could be made more clear.
		FVector InputVector( AxisInput.Y, AxisInput.X, 0 ); 
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
			InputComponent->GetAxisValue( "MotionControllerThumbLeft_X" ),
			InputComponent->GetAxisValue( "MotionControllerThumbLeft_Y" )
		);

		AVRHand *Right = Cast<AVRHand>( RightHand->GetChildActor() );
		FVector2D ThumbRight(
			InputComponent->GetAxisValue( "MotionControllerThumbRight_X" ),
			InputComponent->GetAxisValue( "MotionControllerThumbRight_Y" )
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
			GetRotationFromInput( ThumbLeft, OrientRotator );
			Left->TeleportRotator = OrientRotator;
		}

		if ( Right->IsTeleporterActive )
		{
			GetRotationFromInput( ThumbRight, OrientRotator );
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

	PlayerInputComponent->BindAxis( TEXT( "MotionControllerThumbLeft_X" ) );
	PlayerInputComponent->BindAxis( TEXT( "MotionControllerThumbLeft_Y" ) );

	PlayerInputComponent->BindAxis( TEXT( "MotionControllerThumbRight_X" ) );
	PlayerInputComponent->BindAxis( TEXT( "MotionControllerThumbRight_Y" ) );
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