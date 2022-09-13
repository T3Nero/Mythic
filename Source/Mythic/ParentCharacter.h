// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "ParentCharacter.generated.h"

USTRUCT(BlueprintType)
struct FBaseStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxMP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Adrenaline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Intelligence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Constitution;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Wisdom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PhysicalArmour;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ElementalArmour;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PhysicalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ElementalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 HealingPotency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CriticalChance;
	
};

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

	void AttackCombo();

	void InitializeBaseStats();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bCriticalHit);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool bWeaponDrawn;

	// Check if character is currently crouching under an object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool bCanStand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		ECombatState CombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		int32 ComboIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool bCanAttack;

	// Set at runtime based on equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		float WeaponSpeed;

	// Team Number determines whether an NPC is friendly/enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		int32 TeamNumber;

	// Data Table to use when setting Base Stats Struct values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats", meta = (AllowPrivateAccess = "true"))
		class UDataTable* BaseStatsData;

	// Row Name to look for within Data Table
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats", meta = (AllowPrivateAccess = "true"))
		FName RowName;

	int32 CurrentHP;
	int32 TotalDamageTaken;

public:

	// Called in AnimBP as an Anim Notify or in mythicaniminstance class when crouching
	UFUNCTION(BlueprintCallable)
	void SheatheWeapon();

	void PlayDrawSheatheWeaponMontage();

	// Compares team numbers to check if target is an enemy
	bool IsEnemy(AActor* Target) const;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FBaseStats* BaseStatsStruct;
	bool bIsDead;

	// Public Getters
	FORCEINLINE float GetBaseMovementSpeed() const { return BaseMovementSpeed; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE bool GetStrafing() const { return bStrafing; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetWeaponDrawn() const { return bWeaponDrawn; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE int32 GetComboIndex() const { return ComboIndex; }
	FORCEINLINE bool GetCanAttack() const { return bCanAttack; }
	FORCEINLINE bool GetCanStand() const { return bCanStand; }
	FORCEINLINE float GetWeaponSpeed() const { return WeaponSpeed; }
	FORCEINLINE int32 GetTeamNumber() const { return TeamNumber; }

	// Public Setters
	FORCEINLINE void SetCrouching(bool Crouch) { bCrouching = Crouch; }
	FORCEINLINE void SetWeaponDrawn(bool WeaponDrawn) { bWeaponDrawn = WeaponDrawn; }
	FORCEINLINE void SetEquippedWeapon(AWeapon* Weapon) { EquippedWeapon = Weapon; }
	FORCEINLINE void SetCombatState(ECombatState Combat) { CombatState = Combat; }
	FORCEINLINE void SetComboIndex(int32 Combo) { ComboIndex = Combo; }
	FORCEINLINE void SetCanAttack(bool Attack) { bCanAttack = Attack; }
	FORCEINLINE void SetWeaponSpeed(float Speed) { WeaponSpeed = Speed; }

};
