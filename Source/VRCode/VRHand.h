/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

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

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class USceneComponent *Scene;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent *MotionController;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent *HandMesh;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class USphereComponent *GrabSphere;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class USplineComponent *ArcSpline;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UArrowComponent *ArcDirection;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *ArcEndPoint;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportCylinder;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportRing;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportArrow;
	

	FRotator InitialControllerRotation;
	TArray<class USplineMeshComponent*> SplineMeshes;
public:	

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	class UStaticMesh *BeamMesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	class UMaterial *BeamMaterial;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	class UHapticFeedbackEffect_Base *HapticEffect;

	/** Is this the left or right hand */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	EControllerHand Hand;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float TeleportLaunchVelocity = 900;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	FVector Extents;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Variables" )
	bool WantsToGrip;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Variables" )
	bool IsTeleporterActive;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Variables" )
	EGripState Grip;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Variables" )
	class AActor *AttachedActor;

	FRotator TeleportRotator;
	bool LastIsValidTeleportDestination;
	bool HasValidTeleportLocation;

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

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Default" )
	void GrabActor();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Default" )
	void ReleaseActor();

	AActor* GetActorNearHand();
	void UpdateAnimationGripState();

	void ActivateTeleporter();
	void DisableTeleporter();

	void GetTeleportDestination( FVector &OutPosition, FRotator &OutRotator );
	bool TraceTeleportDestination( TArray<FVector> &TracePoints, FVector &NavMeshLocation, FVector &TraceLocation );

	FRotator GetControllerRelativeRotation();

};
