// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "VRHand.generated.h"

UCLASS()
class VRCODE_API AVRHand : public AActor
{
	GENERATED_BODY()

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent *MotionController;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent *HandMesh;

public:	

	/** Is this the left or right hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	EControllerHand Hand;

	// Sets default values for this actor's properties
	AVRHand();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction( const FTransform & Transform ) override;
	
};
