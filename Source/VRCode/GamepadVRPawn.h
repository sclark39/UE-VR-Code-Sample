/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/


#pragma once

#include "GameFramework/Pawn.h"
#include "GamepadVRPawn.generated.h"

UCLASS()
class VRCODE_API AGamepadVRPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class USceneComponent *Scene;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class USceneComponent *VROrigin;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UCameraComponent *Camera;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UArrowComponent *TraceDirection;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportPin;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportRing;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *TeleportArrow;


	class UMaterialInstanceDynamic *RingGlowMaterialInstance;

	bool CurrentLocationValid;
	bool LocationPinned;
	bool LocationFound;
	FVector PinnedLocation;
	FVector CurrentLookAtLocation;
	FRotator PinnedRotation;

public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	bool bUseGamepad = true;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float FadeInDuration = 0.1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float FadeOutDuration = 0.1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	FLinearColor TeleportFadeColor = FLinearColor::Black;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float DefaultPlayerHeight = 180;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float ThumbDeadzone = 0.1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float RotationLengthThreshold = 100;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	FVector Extents;

	// Sets default values for this pawn's properties
	AGamepadVRPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	bool GetTeleportDirection( FVector &OutLocation );
	void UpdateTeleportDirection( const FVector2D &StickInput );

	UFUNCTION()
	void FinishTeleport();

	void ExecuteTeleport();	
	void ActivateTeleport();

	
};
