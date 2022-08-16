// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParentCharacter.generated.h"

UCLASS()
class MYTHIC_API AParentCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AParentCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// smoothly transitions capsule half height based on standing/crouching for collision purposes
	void InterpCapsuleHalfHeight(float DeltaTime);

	// Change locomotion between strafe and no strafe (will be toggled when locking On/Off Enemy)
	UFUNCTION(BlueprintCallable)
		void ToggleStrafing();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float BaseMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float StandingCapsuleHalfHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchingCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bCrouching;

	// Will be toggled in blueprints when TAB targeting (allows strafing when locked on to enemy)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bStrafing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		bool bWeaponDrawn;

public:

	// Called when Player Crouches or bCrouching = true
	void Crouching();

	// Player/NPC attempts to roll dodge when called
	UFUNCTION(BlueprintImplementableEvent)
		void Dodge();

	// Public Getters
	FORCEINLINE float GetBaseMovementSpeed() const { return BaseMovementSpeed; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE bool GetStrafing() const { return bStrafing; }

	// Public Setters
	FORCEINLINE void SetCrouching(bool Crouch) { bCrouching = Crouch; }
	FORCEINLINE void SetWeaponDrawn(bool WeaponDrawn) { bWeaponDrawn = WeaponDrawn; }

};
