// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "MythicHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYTHIC_API UMythicHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

	// Used to show/hide item pick up widget in parent item  class
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UOverlay* ItemPickUp_Overlay;

	// Set in parent item class on begin play
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* ItemName;
};
