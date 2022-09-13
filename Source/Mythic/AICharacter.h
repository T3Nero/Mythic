// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentCharacter.h"
#include "AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class MYTHIC_API AAICharacter : public AParentCharacter
{
	GENERATED_BODY()

public:
	AAICharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

private:

	// Map to store hit number widgets and hit locations
	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	// Time to display hit number before removing it
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;


	
};
