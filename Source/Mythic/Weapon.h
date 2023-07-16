// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentItem.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 PhysicalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 ElementalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 RequiredStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 RequiredIntelligence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 RequiredSkill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Speed;

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
		float Brutality;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Bleed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Poison;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float LifeSteal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ManaSteal;

};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Null UMETA(DisplayName = "Null"),
	EWT_TwoHand UMETA(DisplayName = "TwoHand"),
	EWT_OneHandShield UMETA(DisplayName = "OneHand&Shield"),
	EWT_DualWield UMETA(DisplayName = "DualWield"),
	EWT_Spear UMETA(DisplayName = "Spear"),
	EWT_Staff UMETA(DisplayName = "Staff"),

	EWT_MAX
};

UCLASS()
class MYTHIC_API AWeapon : public AParentItem
{
	GENERATED_BODY()

public:

	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void InitializeWeaponStats();

	UFUNCTION(BlueprintCallable)
	void DamageDetectionTrace();

	void DoDamage(AActor* ActorHit);

	// Called when doing damage (Chance to knockback & deal 10% increased damage to enemy if weapon has Brutality stat)
	void BrutalityProc(AActor* Receiver);

	// Called in DoDamage() (Chance to apply bleed effect to enemy) ** deals damage over time if weapon has Bleed stat **
	void BleedProc(AActor* Receiver, float Damage);

	// Called in DoDamage() : Chance to apply poison on hit if weapon has poison stat
	void PoisonProc(AActor* Receiver, float Damage);

	// Applies a DOT poison for 5 minutes
	void AgonizingPoison(float Damage);

	void LifeStealProc(float DamageDone);

	void ManaStealProc(AActor* Receiver, float DamageDone);

	// Called when doing damage (adrenaline is required for dodging / ultimate skills)
	void GainAdrenaline();

	// Called using a timer to slowly decrease adrenaline over time
	UFUNCTION()
	void DecreaseAdrenaline();

	// Called when doing damage (each weapon type has its own weapon skill, required for equipping stronger weapons/skills) 
	void GainWeaponSkill() const;

	UFUNCTION(BlueprintCallable)
	void TwoHandExplosion();

	

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		class UDataTable* WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName WeaponDrawnSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName WeaponSheathedSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DrawWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* SheatheWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TraceStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TraceEnd;

	// Checks if actor has been damaged so we only damage once during the current attack (cleared in anim notify after attack)
	UPROPERTY(BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> AlreadyDamagedActors;

	// Knockback Montage to play when Brutality Procs
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockbackMontage;

	
	float DecreaseAdrenalineTick;
	FTimerHandle DecAdrenalineTimerHandle;

	float BrutalityDamage;
	bool bBrutalHit;

	class AParentCharacter* WeaponOwner;
	AParentCharacter* Enemy;
	float SpeedValue;
	TMap<FString, float> WeaponSpeedMap;

	FWeaponStats* WeaponStats;

public:

	bool CheckWeaponRequirements(APlayerCharacter* Char);

	FORCEINLINE FName GetWeaponDrawnSocket() const { return WeaponDrawnSocket; }
	FORCEINLINE FName GetWeaponSheathedSocket() const { return WeaponSheathedSocket; }
	FORCEINLINE UAnimMontage* GetDrawWeaponMontage() const { return DrawWeapon; }
	FORCEINLINE UAnimMontage* GetSheatheWeaponMontage() const { return SheatheWeapon; }
	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	FORCEINLINE float GetWeaponSpeed() const { return SpeedValue; }
	FORCEINLINE FWeaponStats* GetWeaponStats() const { return WeaponStats; }
	FORCEINLINE AParentCharacter* GetWeaponOwner() const { return WeaponOwner; }
	FORCEINLINE bool GetBrutalHit() const { return bBrutalHit; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	
};
