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

USTRUCT(BlueprintType)
struct FWeaponSkill : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WeaponSkill;
	
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

	
	void InitializeWeaponStats();

	UFUNCTION(BlueprintCallable)
	void DamageDetectionTrace();

	void DoDamage(AActor* ActorHit);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		class UDataTable* WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		class UDataTable* WeaponSkillData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		FName WeaponSkillRowName;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DodgeStepMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* TraceStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TraceEnd;

	// Checks if actor has been damaged so we only damage once during the current attack (cleared in anim notify after attack)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> AlreadyDamagedActors;

	class AParentCharacter* WeaponOwner;
	float SpeedValue;
	TMap<FString, float> WeaponSpeedMap;
	FWeaponStats* WeaponStats;

public:

	FWeaponSkill* WeaponSkillStruct;

	FORCEINLINE FName GetWeaponDrawnSocket() const { return WeaponDrawnSocket; }
	FORCEINLINE FName GetWeaponSheathedSocket() const { return WeaponSheathedSocket; }
	FORCEINLINE UAnimMontage* GetDrawWeaponMontage() const { return DrawWeapon; }
	FORCEINLINE UAnimMontage* GetSheatheWeaponMontage() const { return SheatheWeapon; }
	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	FORCEINLINE UAnimMontage* GetDodgeMontage() const { return DodgeStepMontage; }
	FORCEINLINE float GetWeaponSpeed() const { return SpeedValue; }
	FORCEINLINE FWeaponStats* GetWeaponStats() const { return WeaponStats; }
	FORCEINLINE AParentCharacter* GetWeaponOwner() const { return WeaponOwner; }
	
};
