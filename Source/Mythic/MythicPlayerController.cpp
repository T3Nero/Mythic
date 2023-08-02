// Fill out your copyright notice in the Description page of Project Settings.


#include "MythicPlayerController.h"
#include "Blueprint/UserWidget.h"

AMythicPlayerController::AMythicPlayerController()
{

}

void AMythicPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Check HUDOverlay has been set in Blueprint
	if (HUDOverlayClass)
	{
		MythicHUD = CreateWidget<UMythicHUD>(this, HUDOverlayClass);
		if (MythicHUD)
		{
			MythicHUD->AddToViewport();
			MythicHUD->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
