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
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
