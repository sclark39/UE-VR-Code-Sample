#include "TP_VirtualReality.h"
#include "TP_VirtualRealityMotionControllerPawn.h"
#include "TP_VirtualRealityMotionController.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"

// Sets default values
ATP_VirtualRealityMotionControllerPawn::ATP_VirtualRealityMotionControllerPawn()
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
void ATP_VirtualRealityMotionControllerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Setup Player Height for Various Platforms
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	if ( hmd )
	{
		DeviceType = hmd->GetHMDDeviceType();

		// Set up Teleport Style
		if ( ControlScheme == ETeleportControlScheme::Auto )
			ControlScheme =
				( DeviceType == EHMDDeviceType::DT_OculusRift	? ETeleportControlScheme::StickOnly :
				( DeviceType == EHMDDeviceType::DT_Morpheus		? ETeleportControlScheme::ControllerRoll
																: ETeleportControlScheme::ButtonAndStick ) );

		if ( DeviceType == EHMDDeviceType::DT_SteamVR || DeviceType == EHMDDeviceType::DT_OculusRift )
		{
			hmd->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
		}
		else
		{
			// PSVR
			hmd->SetTrackingOrigin(EHMDTrackingOrigin::Eye);

			// Set Height Offset for Tracking			
			VROrigin->AddLocalOffset( FVector( 0, 0, DefaultPlayerHeight ) );
		}
	}

}

void ATP_VirtualRealityMotionControllerPawn::FinishTeleport( ATP_VirtualRealityMotionController *Current, const FVector &TeleportPosition, const FRotator &TeleportRotator )
{
	Current->DisableTeleporter();

	// Move the player
	TeleportTo( TeleportPosition, TeleportRotator, false, false );

	// Fade back in
	APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager( GetWorld(), 0 );
	PlayerCamera->StartCameraFade( 1, 0, FadeInDuration, TeleportFadeColor, false, true );

	// All done.
	IsTeleporting = false;
}

void ATP_VirtualRealityMotionControllerPawn::ExecuteTeleport( ATP_VirtualRealityMotionController *Current )
{
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
	PlayerCamera->StartCameraFade( 0, 1, FadeOutDuration, TeleportFadeColor, false, true );
	
	// Wait for Fade to complete before continuing the teleport
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction( this, FName( TEXT( "FinishTeleport" ) ), Current, TeleportPosition, TeleportRotator );
	GetWorldTimerManager().SetTimer( TimerHandle, TimerDelegate, FadeOutDuration, false );
}


void ATP_VirtualRealityMotionControllerPawn::HandleButtonStyleTeleportActivation( UChildActorComponent *Hand, EInputEvent KeyEvent )
{
	if ( ControlScheme == ETeleportControlScheme::StickOnly )
		return;

	ATP_VirtualRealityMotionController *Current = Cast<ATP_VirtualRealityMotionController>( Hand->GetChildActor() );
	ATP_VirtualRealityMotionController *Other = Cast<ATP_VirtualRealityMotionController>( ( Hand == LeftHand ? RightHand : LeftHand )->GetChildActor() );

	if ( KeyEvent == IE_Pressed )
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

void ATP_VirtualRealityMotionControllerPawn::HandleStickInputStyleTeleportActivation( FVector2D AxisInput, ATP_VirtualRealityMotionController *Current, ATP_VirtualRealityMotionController *Other )
{
	if ( ControlScheme != ETeleportControlScheme::StickOnly )
		return;

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

bool ATP_VirtualRealityMotionControllerPawn::GetRotationFromInput( ATP_VirtualRealityMotionController *Current, FVector2D AxisInput, FRotator &OrientRotator )
{
	FRotator ActorRotator = GetActorRotation();
	ActorRotator.Roll = 0;
	ActorRotator.Pitch = 0;

	if ( ControlScheme == ETeleportControlScheme::ControllerRoll )
	{
		const FRotator RelativeRotation = Current->GetControllerRelativeRotation();
		float FinalYaw = RelativeRotation.Roll * 3 + ActorRotator.Yaw;

		OrientRotator = FRotator( 0, FinalYaw, 0 );
		return true;
	}
	else
	{
		const float ThumbDeadzoneSq = ThumbDeadzone * ThumbDeadzone;
		if ( AxisInput.SizeSquared() > ThumbDeadzoneSq )
		{
			FVector InputVector( AxisInput, 0 );
			InputVector.Normalize();
			InputVector = ActorRotator.RotateVector( InputVector );

			OrientRotator = InputVector.ToOrientationRotator();
			return true;
		}
	}

	OrientRotator = ActorRotator;
	return false;
}

// Called every frame
void ATP_VirtualRealityMotionControllerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ( InputComponent )
	{
		ATP_VirtualRealityMotionController *Left = Cast<ATP_VirtualRealityMotionController>( LeftHand->GetChildActor() );
		FVector2D ThumbLeft(
			InputComponent->GetAxisValue( TEXT( "ThumbLeft_Fwd" ) ),
			InputComponent->GetAxisValue( TEXT( "ThumbLeft_Side" ) )
		);

		ATP_VirtualRealityMotionController *Right = Cast<ATP_VirtualRealityMotionController>( RightHand->GetChildActor() );
		FVector2D ThumbRight(
			InputComponent->GetAxisValue( TEXT( "ThumbRight_Fwd" ) ),
			InputComponent->GetAxisValue( TEXT( "ThumbRight_Side" ) )
		);

		// Robo Rally style Teleport Activation
		HandleStickInputStyleTeleportActivation( ThumbLeft, Left, Right );
		HandleStickInputStyleTeleportActivation( ThumbRight, Right, Left );

		// Get Teleport Target Rotation
		if ( Left->IsTeleporterActive )
		{
			// If there is no Rotational Input, only use Actor rotation if not using Robo Rally style teleport
			FRotator OrientRotator;
			if ( GetRotationFromInput( Left, ThumbLeft, OrientRotator ) || ControlScheme != ETeleportControlScheme::StickOnly )
				Left->TeleportRotator = OrientRotator;
		}

		if ( Right->IsTeleporterActive )
		{
			// If there is no Rotational Input, only use Actor rotation if not using Robo Rally style teleport
			FRotator OrientRotator;
			if ( GetRotationFromInput( Right, ThumbRight, OrientRotator ) || ControlScheme != ETeleportControlScheme::StickOnly )
				Right->TeleportRotator = OrientRotator;
		}
		

	}

}

void ATP_VirtualRealityMotionControllerPawn::HandleGrip( UChildActorComponent *Hand, EInputEvent KeyEvent )
{
	ATP_VirtualRealityMotionController *Current = Cast<ATP_VirtualRealityMotionController>( Hand->GetChildActor() );
	if ( Current )
	{
		if ( KeyEvent == IE_Pressed )
			Current->GrabActor();
		else // released
			Current->ReleaseActor();
	}
}

void ATP_VirtualRealityMotionControllerPawn::BindInputActionUFunction( class UInputComponent* PlayerInputComponent, FName ActionName, EInputEvent KeyEvent, FName FuncName, UChildActorComponent *Hand )
{
	FInputActionBinding InputActionBinding( ActionName, KeyEvent );

	FInputActionHandlerSignature InputActionHandler;
	InputActionHandler.BindUFunction( this, FuncName, Hand, KeyEvent );

	InputActionBinding.ActionDelegate = InputActionHandler;
	PlayerInputComponent->AddActionBinding( InputActionBinding );
}

// Called to bind functionality to input
void ATP_VirtualRealityMotionControllerPawn::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

	BindInputActionUFunction( PlayerInputComponent, TEXT( "TeleportLeft" ), IE_Pressed, TEXT( "HandleButtonStyleTeleportActivation" ), LeftHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "TeleportLeft" ), IE_Released, TEXT( "HandleButtonStyleTeleportActivation" ), LeftHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "TeleportRight" ), IE_Pressed, TEXT( "HandleButtonStyleTeleportActivation" ), RightHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "TeleportRight" ), IE_Released, TEXT( "HandleButtonStyleTeleportActivation" ), RightHand );

	BindInputActionUFunction( PlayerInputComponent, TEXT( "GripLeft" ), IE_Pressed, TEXT( "HandleGrip" ), LeftHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "GripLeft" ), IE_Released, TEXT( "HandleGrip" ), LeftHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "GrabLeft" ), IE_Pressed, TEXT( "HandleGrip" ), LeftHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "GrabLeft" ), IE_Released, TEXT( "HandleGrip" ), LeftHand );

	BindInputActionUFunction( PlayerInputComponent, TEXT( "GripRight" ), IE_Pressed, TEXT( "HandleGrip" ), RightHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "GripRight" ), IE_Released, TEXT( "HandleGrip" ), RightHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "GrabRight" ), IE_Pressed, TEXT( "HandleGrip" ), RightHand );
	BindInputActionUFunction( PlayerInputComponent, TEXT( "GrabRight" ), IE_Released, TEXT( "HandleGrip" ), RightHand );

	PlayerInputComponent->BindAxis( TEXT( "ThumbLeft_Fwd" ) );
	PlayerInputComponent->BindAxis( TEXT( "ThumbRight_Fwd" ) );

	PlayerInputComponent->BindAxis( TEXT( "ThumbLeft_Side" ) );
	PlayerInputComponent->BindAxis( TEXT( "ThumbRight_Side" ) );
}
