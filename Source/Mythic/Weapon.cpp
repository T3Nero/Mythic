// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "ParentCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"

AWeapon::AWeapon() :
	WeaponType(EWeaponType::EWT_Null),
	DecreaseAdrenalineTick(5.f)
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

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
}

// Sphere trace to check if weapon swing collided with an Enemy
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
					bIsTwoHandExplosion = false;
					AlreadyDamagedActors.AddUnique(ActorHit);
					DoDamage(ActorHit);
				}
			}
		}
	}
}

// Called when dealing damage to an enemy hit by detection trace
void AWeapon::DoDamage(AActor* ActorHit)
{
	const float PhysicalDamage = WeaponStats->PhysicalDamage;
	const float ElementalDamage = WeaponStats->ElementalDamage;
	const float CharacterDamage = (WeaponOwner->BaseStatsStruct->PhysicalDamage + WeaponOwner->BaseStatsStruct->ElementalDamage);

	float TotalDamage = (PhysicalDamage + ElementalDamage + CharacterDamage);

	BrutalityProc(ActorHit);
	if(bBrutalHit)
	{
		TotalDamage *= BrutalityDamage;
	}

	GainAdrenaline();
	GainWeaponSkill();

	UGameplayStatics::ApplyDamage(ActorHit, TotalDamage, GetInstigatorController(), this, UDamageType::StaticClass());

	if(!bIsTwoHandExplosion)
	{
		BleedProc(ActorHit, TotalDamage);
		PoisonProc(ActorHit, TotalDamage);
		LifeStealProc(TotalDamage);
		ManaStealProc(ActorHit, TotalDamage);
	}
}

// Does 10% increased damage & knocks back enemy
void AWeapon::BrutalityProc(AActor* Receiver)
{
	Enemy = Cast<AParentCharacter>(Receiver);
	if(Enemy)
	{
		const float ProcValue {FMath::RandRange(0.f, 100.f)};
		if(ProcValue < WeaponStats->Brutality)
		{
			UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
			if(AnimInstance && KnockbackMontage)
			{
				// 10% increased damage
				BrutalityDamage = 1.1f;
				bBrutalHit = true;
				AnimInstance->Montage_Play(KnockbackMontage);
				Enemy->SetCombatState(ECombatState::ECS_Stunned);
			}
		}
		else
		{
			BrutalityDamage = 0;
			bBrutalHit = false;
		}
	}
}

void AWeapon::BleedProc(AActor* Receiver, float Damage)
{
	Enemy = Cast<AParentCharacter>(Receiver);
	if(Enemy)
	{
		const float ProcValue {FMath::RandRange(0.f, 100.f)};
		if(ProcValue <= WeaponStats->Bleed)
		{
			// Apply Bleed
			// Bleed is applied to damaged actor (sets a looping timer for damage over time)
			Enemy->ApplyBleed(Damage, this);
		}
	}
}


void AWeapon::PoisonProc(AActor* Receiver, float Damage)
{
	Enemy = Cast<AParentCharacter>(Receiver);
	if(Enemy)
	{
		const float ProcValue {FMath::RandRange(0.f, 100.f)};
		if(ProcValue <= WeaponStats->Poison)
		{
			// Apply Poison
			AgonizingPoison(Damage);
		}
	}
}

void AWeapon::AgonizingPoison(float Damage)
{
	Enemy->ApplyPoison(Damage, this);
}

void AWeapon::LifeStealProc(float DamageDone)
{
	if(WeaponOwner)
	{
		const float ProcValue {FMath::RandRange(0.f, 100.f)};
		if(ProcValue <= WeaponStats->LifeSteal)
		{
			// Heal Weapon Owner for 10% Damage Done
			if(WeaponOwner->GetCurrentHP() < WeaponOwner->BaseStatsStruct->MaxHP)
			{
				int32 HP = WeaponOwner->GetCurrentHP();
				WeaponOwner->SetCurrentHP(HP += (DamageDone * 0.1f));
				WeaponOwner->SetCurrentHP(FMath::Clamp(WeaponOwner->GetCurrentHP(), 0, WeaponOwner->BaseStatsStruct->MaxHP));
			}
		}
	}
}

void AWeapon::ManaStealProc(AActor* Receiver, float DamageDone)
{
	if(WeaponOwner)
	{
		const float ProcValue {FMath::RandRange(0.f, 100.f)};
		if(ProcValue <= WeaponStats->ManaSteal)
		{
			// Regain MP for 10% Damage Done
			if(WeaponOwner->GetCurrentMP() < WeaponOwner->BaseStatsStruct->MaxMP)
			{
				int32 MP = WeaponOwner->GetCurrentMP();
				WeaponOwner->SetCurrentMP(MP += (DamageDone * 0.1f));
				WeaponOwner->SetCurrentMP(FMath::Clamp(WeaponOwner->GetCurrentMP(), 0, WeaponOwner->BaseStatsStruct->MaxMP));

				Enemy = Cast<AParentCharacter>(Receiver);
				if(Enemy)
				{
					if(Enemy->GetCurrentMP() > 0)
					{
						int32 EnemyMP = Enemy->GetCurrentMP();
						Enemy->SetCurrentMP(EnemyMP -= (DamageDone * 0.1f));
						Enemy->SetCurrentMP(FMath::Clamp(Enemy->GetCurrentMP(), 0, Enemy->BaseStatsStruct->MaxMP));
					}
				}
			}
		}
	}
}

void AWeapon::GainAdrenaline()
{
	if(WeaponOwner)
	{
		// amount of adrenaline gained per hit is increased based on players dexterity (capped at 20)
		float AdrenalinePerHit = (0.2 * WeaponOwner->BaseStatsStruct->Dexterity);
		AdrenalinePerHit = FMath::Clamp(AdrenalinePerHit, 0, 20);
		AdrenalinePerHit = FMath::RoundToInt32(AdrenalinePerHit);

		if(WeaponOwner->BaseStatsStruct->Adrenaline < 100)
		{
			WeaponOwner->BaseStatsStruct->Adrenaline += AdrenalinePerHit;
			WeaponOwner->BaseStatsStruct->Adrenaline = FMath::Clamp(WeaponOwner->BaseStatsStruct->Adrenaline, 0, 100);
		}
		GetWorldTimerManager().SetTimer(DecAdrenalineTimerHandle, this, &AWeapon::DecreaseAdrenaline, DecreaseAdrenalineTick, true);
	}
}

void AWeapon::DecreaseAdrenaline()
{
	if(WeaponOwner)
	{
		if(WeaponOwner->BaseStatsStruct->Adrenaline > 0)
		{
			// time it takes for adrenaline to reduce to 0 from max value (100)
			constexpr int32 TimeInSecondsTillMinValue = 300;
			float DecreaseAmountPerTick = 100 * (DecreaseAdrenalineTick / TimeInSecondsTillMinValue);
			DecreaseAmountPerTick = FMath::RoundToInt32(DecreaseAmountPerTick);

			WeaponOwner->BaseStatsStruct->Adrenaline -= DecreaseAmountPerTick;
		}
		else
		{
			GetWorldTimerManager().ClearTimer(DecAdrenalineTimerHandle);
			DecAdrenalineTimerHandle.Invalidate();
		}
	}
}

void AWeapon::GainWeaponSkill() const
{
	// amount of weapon skill gained per hit is increased based on players dexterity (capped at 5)
	float WeaponSkillPerHit = (0.05 * WeaponOwner->BaseStatsStruct->Dexterity);
	WeaponSkillPerHit = FMath::Clamp(WeaponSkillPerHit, 0, 5);

	switch (WeaponType)
	{
	case EWeaponType::EWT_TwoHand:
		WeaponOwner->BaseStatsStruct->TwoHandSkill += WeaponSkillPerHit;
		WeaponOwner->BaseStatsStruct->TwoHandSkill = FMath::Clamp(WeaponOwner->BaseStatsStruct->TwoHandSkill, 0, 200);
		break;
	case EWeaponType::EWT_OneHandShield:
		WeaponOwner->BaseStatsStruct->OneHandShieldSkill += WeaponSkillPerHit;
		WeaponOwner->BaseStatsStruct->OneHandShieldSkill = FMath::Clamp(WeaponOwner->BaseStatsStruct->OneHandShieldSkill, 0, 200);
		break;
	case EWeaponType::EWT_DualWield:
		WeaponOwner->BaseStatsStruct->DualWieldSkill += WeaponSkillPerHit;
		WeaponOwner->BaseStatsStruct->DualWieldSkill = FMath::Clamp(WeaponOwner->BaseStatsStruct->DualWieldSkill, 0, 200);
		break;
	case EWeaponType::EWT_Spear:
		WeaponOwner->BaseStatsStruct->SpearSkill += WeaponSkillPerHit;
		WeaponOwner->BaseStatsStruct->SpearSkill = FMath::Clamp(WeaponOwner->BaseStatsStruct->SpearSkill, 0, 200);
		break;
	case EWeaponType::EWT_Staff:
		WeaponOwner->BaseStatsStruct->StaffSkill += WeaponSkillPerHit;
		WeaponOwner->BaseStatsStruct->StaffSkill = FMath::Clamp(WeaponOwner->BaseStatsStruct->StaffSkill, 0, 200);
		break;
	default:
		break;
	}
}

// Two Hand Combo final attack causes an explosion which damages enemies within its radius
void AWeapon::TwoHandExplosion()
{
	const ETraceTypeQuery TraceParams = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);
	
	TArray<FHitResult> Hits;

	const FVector Location = TraceStart->GetComponentLocation();

	bool bHits = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Location, Location, 500.f, TraceParams, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hits, true, FColor::Blue, FColor::Green, 2.0f);
	if(bHits)
	{
		WeaponOwner = Cast<AParentCharacter>(GetAttachParentActor());
		if(WeaponOwner)
		{
			for (const FHitResult Hit : Hits)
			{
				AActor* ActorHit;
				ActorHit = Hit.GetActor();
				if(WeaponOwner->IsEnemy(ActorHit))
				{
					if(AlreadyDamagedActors.Contains(ActorHit) == false)
					{
						bIsTwoHandExplosion = true;
						AlreadyDamagedActors.AddUnique(ActorHit);
						DoDamage(ActorHit);
					}
				}
			}
		}
	}


}

bool AWeapon::CheckWeaponRequirements(APlayerCharacter* Char)
{

	if(Char->BaseStatsStruct->Strength >= WeaponStats->RequiredStrength && Char->BaseStatsStruct->Intelligence >= WeaponStats->RequiredIntelligence)
	{
		switch (WeaponType)
		{
		case EWeaponType::EWT_TwoHand:
			if(Char->BaseStatsStruct->TwoHandSkill >= WeaponStats->RequiredSkill)
			{
				return true;
			}
			break;
		case EWeaponType::EWT_OneHandShield:
			if(Char->BaseStatsStruct->OneHandShieldSkill >= WeaponStats->RequiredSkill)
			{
				return true;
			}
			break;
		case EWeaponType::EWT_DualWield:
			if(Char->BaseStatsStruct->DualWieldSkill >= WeaponStats->RequiredSkill)
			{
				return true;
			}
			break;
		case EWeaponType::EWT_Spear:
			if(Char->BaseStatsStruct->SpearSkill >= WeaponStats->RequiredSkill)
			{
				return true;
			}
			break;
		case EWeaponType::EWT_Staff:
			if(Char->BaseStatsStruct->StaffSkill >= WeaponStats->RequiredSkill)
			{
				return true;
			}
			break;
		default:
			break;
		}
	}
	

	return false;
}

