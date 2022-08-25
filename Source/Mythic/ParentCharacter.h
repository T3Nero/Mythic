// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParentCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_DrawWeapon UMETA(DisplayName = "Draw Weapon"),
	ECS_SheatheWeapon UMETA(DisplayName = "Sheathe Weapon"),
	ECS_Dodging UMETA(DisplayName = "Dodging"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	
	ECS_MAX
};

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

	// Called in AnimBP as an Anim Notify
	UFUNCTION(BlueprintCallable)
	void DrawWeapon();

	// Sets CombatState to Unoccupied (Called as an AnimNotify at the end of Montages)
	// Allows player/npc to use next action (attack, dodge etc.)
	UFUNCTION(BlueprintCallable)
	void SetUnoccupied();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		class AParentItem* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		ECombatState CombatState;

public:

	// Called in AnimBP as an Anim Notify
	UFUNCTION(BlueprintCallable)
	void SheatheWeapon();

	void PlayDrawSheatheWeaponMontage();

	// Player/NPC attempts to roll dodge when called in Blueprints
	UFUNCTION(BlueprintImplementableEvent)
	void Dodge();


	// Public Getters
	FORCEINLINE float GetBaseMovementSpeed() const { return BaseMovementSpeed; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE bool GetStrafing() const { return bStrafing; }
	FORCEINLINE AParentItem* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetWeaponDrawn() const { return bWeaponDrawn; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	// Public Setters
	FORCEINLINE void SetCrouching(bool Crouch) { bCrouching = Crouch; }
	FORCEINLINE void SetWeaponDrawn(bool WeaponDrawn) { bWeaponDrawn = WeaponDrawn; }
	FORCEINLINE void SetEquippedWeapon(AParentItem* Weapon) { EquippedWeapon = Weapon; }
	FORCEINLINE void SetCombatState(ECombatState Combat) { CombatState = Combat; }

};
