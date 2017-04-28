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

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class USphereComponent *GrabSphere;

public:	
	/** Motion controller (left hand) */
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	bool shouldGrip;

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
	
	void OnComponentBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	void OnComponentHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION( BlueprintNativeEvent )
	void RumbleController( float intensity );
};
