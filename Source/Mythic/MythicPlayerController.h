// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MythicPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYTHIC_API AMythicPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMythicPlayerController();

protected:

	virtual void BeginPlay() override;

private:

	// Reference to player HUD Overlay Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> HUDOverlayClass;

	// Set in Blueprint from HUDOverlayClass
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UUserWidget* HUDOverlay;
};
