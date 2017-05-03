/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

#pragma once

#include "IPickupable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class VRCODE_API IPickupable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface" )
	void Pickup( class USceneComponent *AttachTo );

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface" )
	void Drop();
	
};
