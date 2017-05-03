/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

#pragma once

#include "IPickupable.h"
#include "GameFramework/Actor.h"
#include "PickupCube.generated.h"

UCLASS()
class VRCODE_API APickupCube : public AActor, public IPickupable
{
	GENERATED_BODY()

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = ( AllowPrivateAccess = "true" ) )
	class UStaticMeshComponent *StaticMeshComponent;

public:	
	// Sets default values for this actor's properties
	APickupCube();

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
