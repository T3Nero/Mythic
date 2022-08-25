// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYTHIC_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Creates a pure c++ function to be used in classes which implement this interface (interacting with items, npc's etc.)
	// Called InteractPure as it can only be used in c++ classes
	// Adding " = 0 " because we are not defining the function in this interface .cpp file
	// C++ only
	//UFUNCTION()
	//virtual void InteractPure() = 0;

	// Blueprint Event
	UFUNCTION(BlueprintNativeEvent)
	void Interact();
};
