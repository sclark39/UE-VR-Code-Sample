/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

#pragma once

#include "GameFramework/Pawn.h"

#include "VRPawn.generated.h"

UCLASS()
class VRCODE_API AVRPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent *VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *LeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *RightHand;

	void HandleStickInputStyleTeleportActivation( FVector2D AxisInput, class AVRHand *Current, class AVRHand *Other );
	bool GetRotationFromInput( FVector2D AxisInput, FRotator &OrientRotator );

public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float FadeInDuration = 0.1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float FadeOutDuration = 0.1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	FLinearColor TeleportFadeColor = FLinearColor::Black;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Code Constants" )
	float ThumbDeadzone = 0.7;

	uint8 DeviceType;
	bool IsTeleporting;

	// Sets default values for this pawn's properties
	AVRPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* InputComponent ) override;
	
	void UpdateGrip( UChildActorComponent *hand, bool pressed );

	void GripLeft();
	void StopGripLeft();
	void GripRight();
	void StopGripRight();

	UFUNCTION()
	void FinishTeleport( class AVRHand *Current, const FVector &TeleportPosition, const FRotator &TeleportRotator );

	void ExecuteTeleport( class AVRHand *Current );

	
};
