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

}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	// Setup Player Height for Various Platforms
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	if (hmd)
	{
		EHMDDeviceType::Type devtype = hmd->GetHMDDeviceType();

		if (devtype == EHMDDeviceType::DT_SteamVR || devtype == EHMDDeviceType::DT_OculusRift)
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

// Called every frame
void AVRPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(class UInputComponent* inputComponent)
{
	Super::SetupPlayerInputComponent(inputComponent);

	inputComponent->BindAction( "GripLeft", IE_Pressed, this, &AVRPawn::GripLeft );
	inputComponent->BindAction( "GripLeft", IE_Released, this, &AVRPawn::StopGripLeft );

	inputComponent->BindAction( "GripRight", IE_Pressed, this, &AVRPawn::GripRight );
	inputComponent->BindAction( "GripRight", IE_Released, this, &AVRPawn::StopGripRight );
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