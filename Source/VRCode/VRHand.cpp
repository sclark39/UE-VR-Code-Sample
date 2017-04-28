// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCode.h"
#include "VRHand.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"


// Sets default values
AVRHand::AVRHand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hand = EControllerHand::Right;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>( TEXT( "MotionController" ) );
	MotionController->SetupAttachment( RootComponent );
	MotionController->Hand = Hand;

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "HandMesh" ) );
	HandMesh->SetupAttachment( MotionController );
	

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

// Called when the game starts or when spawned
void AVRHand::BeginPlay()
{
	MotionController->Hand = Hand;
	Super::BeginPlay();
	
}

// Called every frame
void AVRHand::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

