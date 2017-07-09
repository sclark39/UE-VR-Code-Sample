/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

#pragma once

#include "TP_VirtualRealityPickupable.h"
#include "GameFramework/Actor.h"
#include "TP_VirtualRealityPickupCube.generated.h"

UCLASS()
class TP_VIRTUALREALITY_API ATP_VirtualRealityPickupCube : public AActor, public ITP_VirtualRealityPickupable
{
	GENERATED_BODY()

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *StaticMeshComponent;

public:	
	// Sets default values for this actor's properties
	ATP_VirtualRealityPickupCube();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

// 	
// 	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface" )
// 	void Pickup( class USceneComponent *AttachTo );
	virtual void Pickup_Implementation( class USceneComponent *AttachTo );
// 
 	//UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface" )
// 	void Drop();
	virtual void Drop_Implementation();

};
