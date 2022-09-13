// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "ParentCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Trace Start"));
	TraceStart->SetupAttachment(RootComponent);

	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Trace End"));
	TraceEnd->SetupAttachment(RootComponent);



	WeaponSpeedMap.Add(FString("A"), 1.4f);
	WeaponSpeedMap.Add(FString("B"), 1.2f);
	WeaponSpeedMap.Add(FString("C"), 1.0f);
	WeaponSpeedMap.Add(FString("D"), 0.8f);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	InitializeWeaponStats();
	
}

void AWeapon::InitializeWeaponStats()
{
	if(WeaponData)
	{
		const FString ContextString = RowName.ToString();
		WeaponStats = WeaponData->FindRow<FWeaponStats>(RowName, ContextString, true);

		if(WeaponStats)
		{
			if(WeaponSpeedMap.Contains(WeaponStats->Speed))
			{
				SpeedValue = WeaponSpeedMap[WeaponStats->Speed];
			}
		}
	}

	if(WeaponSkillData)
	{
		const FString ConString = WeaponSkillRowName.ToString();
		WeaponSkillStruct = WeaponSkillData->FindRow<FWeaponSkill>(WeaponSkillRowName, ConString, true);
	}
}

void AWeapon::DamageDetectionTrace()
{
	const ETraceTypeQuery TraceParams = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);
	FHitResult Hit;

	const FVector StartLocation = TraceStart->GetComponentLocation();
	const FVector EndLocation = TraceEnd->GetComponentLocation();

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, 20.f, TraceParams, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true, FColor::Blue, FColor::Green, 2.0f);
	if(bHit)
	{
		WeaponOwner = Cast<AParentCharacter>(GetAttachParentActor());
		if(WeaponOwner)
		{
			AActor* ActorHit;
			ActorHit = Hit.GetActor();
			if(WeaponOwner->IsEnemy(ActorHit))
			{
				if(AlreadyDamagedActors.Contains(ActorHit) == false)
				{
					AlreadyDamagedActors.AddUnique(ActorHit);
					DoDamage(ActorHit);
				}
			}
		}
	}
}

void AWeapon::DoDamage(AActor* ActorHit)
{
	const float PhysicalDamage = WeaponStats->PhysicalDamage;
	const float ElementalDamage = WeaponStats->ElementalDamage;
	const float CharacterDamage = (WeaponOwner->BaseStatsStruct->PhysicalDamage + WeaponOwner->BaseStatsStruct->ElementalDamage);

	const float TotalDamage = (PhysicalDamage + ElementalDamage + CharacterDamage);

	UGameplayStatics::ApplyDamage(ActorHit, TotalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
}
