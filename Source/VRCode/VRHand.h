// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "VRHand.generated.h"

UENUM(BlueprintType)
enum class EGripState : uint8
{
	Open,
	CanGrab,
	Grab
};

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

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class USplineComponent *ArcSpline;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class UArrowComponent *ArcDirection;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *ArcEndPoint;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportCylinder;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportRing;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportArrow;

public:	
	/** Motion controller (left hand) */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Default" )
	EGripState Grip;

	/** Is this the left or right hand */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Default" )
	EControllerHand Hand;

	// Sets default values for this actor's properties
	AVRHand();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction( const FTransform & Transform ) override;
	
	UFUNCTION()
	void OnComponentBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Default" )
	void RumbleController( float intensity );
};
