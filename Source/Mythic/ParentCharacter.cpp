// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapon.h"
#include "Blueprint/UserWidget.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Particles/ParticleSystem.h"



// Sets default values
AParentCharacter::AParentCharacter() :
	BaseMovementSpeed(600.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	bCrouching(false),
	bStrafing(false),
	TeamNumber(0),
	bCanStand(true),
	ComboIndex(0),
	bCanAttack(true),
	WeaponSpeed(1.f),
	bIsDead(false)

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Yaw Rotation to True when Strafing (Locked on to Target Enemy)
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Character rotates towards direction of input	// Set bOrientRotationToMovement to False when Strafing (Locked on to Target Enemy)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);
}

// Called when the game starts or when spawned
void AParentCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetUnoccupied();
	InitializeBaseStats();
	CurrentHP = BaseStatsStruct->MaxHP;
	CurrentMP = BaseStatsStruct->MaxMP;

	// Sets a base movement speed so it can be reset if characters speed is changed through being slowed etc.
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
}

// Spawns blood splatter when character hit
void AParentCharacter::SpawnBlood() const
{
	if(BloodSplatter)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += 50;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodSplatter, SpawnLocation);
	}
}

// Applies bleeding damage over time
// Character bleeds for 20% of damage dealt to them over 20 seconds (ticks every 2 seconds)
// Called from ApplyBleed() function
void AParentCharacter::BleedingDOT(float Damage, AWeapon* DamageCauser)
{
	// Ticks a total of 10 times before timer is cleared
	if(BleedIndex < 10)
	{
		++BleedIndex;
		bBleedApplied = true;

		Damage *= 0.2f;
		BleedDamage = Damage *= 0.1f;
		UGameplayStatics::ApplyDamage(this, BleedDamage, GetInstigatorController(), DamageCauser, UDamageType::StaticClass());
	}
	else
	{
		bBleedApplied = false;
		BleedDelegate.Unbind();
		GetWorldTimerManager().ClearTimer(BleedTimerHandle);
		BleedTimerHandle.Invalidate();
	}
}

// Deals 60% of damage dealt over 2 minutes (ticks every second)
void AParentCharacter::PoisonDOT(float Damage, AWeapon* DamageCauser)
{
	// Ticks for 2 minutes total
	if(PoisonIndex < 120)
	{
		++PoisonIndex;
		bPoisonApplied = true;

		Damage *= 0.05f;
		PoisonDamage = Damage *= 0.1f;
		UGameplayStatics::ApplyDamage(this, PoisonDamage, GetInstigatorController(), DamageCauser, UDamageType::StaticClass());
	}
	else
	{
		bPoisonApplied = false;
		PoisonDelegate.Unbind();
		GetWorldTimerManager().ClearTimer(PoisonTimerHandle);
		PoisonTimerHandle.Invalidate();
	}
}

void AParentCharacter::UpdateAttributes(bool bEquippingItem) const
{
	if(EquippedWeapon)
	{
		if(bEquippingItem)
		{
			BaseStatsStruct->Strength += EquippedWeapon->GetWeaponStats()->Strength;
			BaseStatsStruct->Intelligence += EquippedWeapon->GetWeaponStats()->Intelligence;
			BaseStatsStruct->Constitution += EquippedWeapon->GetWeaponStats()->Constitution;
			BaseStatsStruct->Wisdom += EquippedWeapon->GetWeaponStats()->Wisdom;
			BaseStatsStruct->Dexterity += EquippedWeapon->GetWeaponStats()->Dexterity;

			BaseStatsStruct->PhysicalDamage += EquippedWeapon->GetWeaponStats()->Strength * 2.0f;
			BaseStatsStruct->PhysicalArmour += EquippedWeapon->GetWeaponStats()->Constitution * 1.0f;
			BaseStatsStruct->ElementalDamage += EquippedWeapon->GetWeaponStats()->Intelligence * 1.0f;
			BaseStatsStruct->ElementalArmour += EquippedWeapon->GetWeaponStats()->Wisdom * 1.0f;
			BaseStatsStruct->HealingPotency += EquippedWeapon->GetWeaponStats()->Intelligence * 1.0f;
			BaseStatsStruct->MaxHP += EquippedWeapon->GetWeaponStats()->Constitution * 20.0f;
			BaseStatsStruct->MaxMP += EquippedWeapon->GetWeaponStats()->Wisdom * 20.0f;
			BaseStatsStruct->CriticalChance += EquippedWeapon->GetWeaponStats()->Dexterity * 0.5f;
		}
		else
		{
			BaseStatsStruct->Strength -= EquippedWeapon->GetWeaponStats()->Strength;
			BaseStatsStruct->Intelligence -= EquippedWeapon->GetWeaponStats()->Intelligence;
			BaseStatsStruct->Constitution -= EquippedWeapon->GetWeaponStats()->Constitution;
			BaseStatsStruct->Wisdom -= EquippedWeapon->GetWeaponStats()->Wisdom;
			BaseStatsStruct->Dexterity -= EquippedWeapon->GetWeaponStats()->Dexterity;

			BaseStatsStruct->PhysicalDamage -= EquippedWeapon->GetWeaponStats()->Strength * 2.0f;
			BaseStatsStruct->PhysicalArmour -= EquippedWeapon->GetWeaponStats()->Constitution * 1.0f;
			BaseStatsStruct->ElementalDamage -= EquippedWeapon->GetWeaponStats()->Intelligence * 1.0f;
			BaseStatsStruct->ElementalArmour -= EquippedWeapon->GetWeaponStats()->Wisdom * 1.0f;
			BaseStatsStruct->HealingPotency -= EquippedWeapon->GetWeaponStats()->Intelligence * 1.0f;
			BaseStatsStruct->MaxHP -= EquippedWeapon->GetWeaponStats()->Constitution * 20.0f;
			BaseStatsStruct->MaxMP -= EquippedWeapon->GetWeaponStats()->Wisdom * 20.0f;
			BaseStatsStruct->CriticalChance -= EquippedWeapon->GetWeaponStats()->Dexterity * 0.5f;
		}
	}
}

// Called every frame
void AParentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bCrouching)
	{
		InterpCapsuleHalfHeight(DeltaTime);
	}
}

// Reduces characters capsule collider when crouching : allows for crouching under objects & cannot stand back up if they are currently under an object
void AParentCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching && !GetCharacterMovement()->IsFalling())
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;

		FVector Start = GetActorLocation();
		FVector End = GetActorLocation();
		End.Z += 200;

		ETraceTypeQuery TraceParams = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		FHitResult Hit;

		// Does a line trace upwards from character location to check if character is crouching under an object
		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceParams, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);
		if (bHit)
		{
			bCanStand = false;
		}
		else
		{
			bCanStand = true;
		}

		if(EquippedWeapon)
		{
			SheatheWeapon();
		}
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f) };

	// Negative float if crouching; Positive if standing // Offsets Mesh Location so it does not pass through ground when crouched
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };

	GetMesh()->AddLocalOffset(MeshOffset);
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight, true);
}

void AParentCharacter::ToggleStrafing()
{
	if (bStrafing)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

// Attaches weapon to characters drawn weapon socket : Called in AnimBP as an anim notify
void AParentCharacter::DrawWeapon()
{
	if (EquippedWeapon == nullptr) { return; }

	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(FName(EquippedWeapon->GetWeaponDrawnSocket()));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, GetMesh());
	}
	bWeaponDrawn = true;
}

// Characters cannot perform next action until their CombatState is set to Unoccupied
void AParentCharacter::SetUnoccupied()
{
	// Resets Attack
	ComboIndex = 0;
	bCanAttack = true;

	// Dodge ends, character can no longer pass through other characters and can be hit
	// During dodge, character is invincible
	if(CombatState == ECombatState::ECS_Dodging)
	{
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
	}

	CombatState = ECombatState::ECS_Unoccupied;
}

// Attaches weapon to characters sheathe weapon socket : Called in AnimBP as an anim notify 
void AParentCharacter::SheatheWeapon()
{
	if (EquippedWeapon == nullptr) { return; }

	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(FName(EquippedWeapon->GetWeaponSheathedSocket()));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, GetMesh());
	}
	bWeaponDrawn = false;
}

// Toggles draw/sheathe weapon animation 
void AParentCharacter::PlayDrawSheatheWeaponMontage()
{
	if (EquippedWeapon == nullptr) { return; }
	if (bCrouching || GetCharacterMovement()->IsFalling()) { return; }
	if (CombatState != ECombatState::ECS_Unoccupied) { return; }

	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (bWeaponDrawn)
		{
			if (GetEquippedWeapon()->GetSheatheWeaponMontage())
			{
				AnimInstance->Montage_Play(GetEquippedWeapon()->GetSheatheWeaponMontage());
				CombatState = ECombatState::ECS_SheatheWeapon;
				bStrafing = false;
			}
		}
		else
		{
			if (GetEquippedWeapon()->GetDrawWeaponMontage())
			{
				AnimInstance->Montage_Play(GetEquippedWeapon()->GetDrawWeaponMontage());
				CombatState = ECombatState::ECS_DrawWeapon;
				bStrafing = true;
			}
		}
		ToggleStrafing();
	}
}

// Weapons may have a chance of applying a bleed effect when character is hit
void AParentCharacter::ApplyBleed(float Damage, AWeapon* DamageCauser)
{
	if(!bBleedApplied)
	{
		// BleedingDOT ticks every 2 seconds
		constexpr float BleedTick = 2.0f;

		BleedIndex = 0;
		BleedDelegate.BindUFunction(this, "BleedingDOT", Damage, DamageCauser);
		GetWorldTimerManager().SetTimer(BleedTimerHandle, BleedDelegate, BleedTick, true);
	}
}

void AParentCharacter::ApplyPoison(float Damage, AWeapon* DamageCauser)
{
	if(!bPoisonApplied)
	{
		constexpr  float PoisonTick = 1.0f;

		PoisonIndex = 0;
		PoisonDelegate.BindUFunction(this, "PoisonDOT", Damage, DamageCauser);
		GetWorldTimerManager().SetTimer(PoisonTimerHandle, PoisonDelegate, PoisonTick, true);
	}
}

// Check to see if target is an enemy
// Returns true if other character is an enemy
bool AParentCharacter::IsEnemy(AActor* Target) const
{
	const AParentCharacter* Enemy = Cast<AParentCharacter>(Target);
	if(Enemy)
	{
		if(TeamNumber != Enemy->GetTeamNumber())
		{
			return true;
		}
	}
	return false;
}

// Called any time ApplyDamage() function is being called
float AParentCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	bool bNormalHit = false;

	// Weapon doing the damage
	const AWeapon* WeaponEquipped = Cast<AWeapon>(DamageCauser);
	const AParentCharacter* WeaponOwner = Cast<AParentCharacter>(WeaponEquipped->GetWeaponOwner());
	if(WeaponEquipped && WeaponOwner)
	{
		if(DamageAmount == BleedDamage || DamageAmount == PoisonDamage)
		{
			TotalDamageTaken = DamageAmount;
			bNormalHit = false;
		}
		else
		{
			TotalDamageTaken = (DamageAmount - BaseStatsStruct->PhysicalArmour - BaseStatsStruct->ElementalArmour);
			bNormalHit = true;
		}

		// Increases damage done during Combo Attack
		for (int32 i = 0; i <= WeaponOwner->GetComboIndex(); i++)
		{
			if(WeaponOwner->GetComboIndex() == 0)
			{
				i = 0;
			}
			else
			{
				TotalDamageTaken *= 1.2f; // Each hits damage increased by 20% during combo
			}
		}
	}

	// Actor taking damage
	if(BaseStatsStruct)
	{
		TotalDamageTaken = FMath::Clamp(TotalDamageTaken, 1, TotalDamageTaken);
		bool bCriticalHit;
		SpawnBlood();

		// Critical Hit
		if(FMath::RandRange(0, 100) <= WeaponOwner->BaseStatsStruct->CriticalChance)
		{
			TotalDamageTaken *= 2;
			bCriticalHit = true;
		}
		else
		{
			bCriticalHit = false;
		}

		// Dead
		if(CurrentHP - TotalDamageTaken <= 0.f)
		{
			CurrentHP = 0;
			bIsDead = true;
		}
		else
		{
			CurrentHP -= TotalDamageTaken;

		}

		// Shows hit numbers above enemy head
		if(!IsPlayerControlled())
		{
			FVector HitLocation = GetActorLocation();
			HitLocation.Z += FMath::RandRange(80, 100);

			if(bNormalHit)
			{
				ShowHitNumber(TotalDamageTaken, HitLocation, bCriticalHit, WeaponEquipped->GetBrutalHit());
			}

		}
	}

	return DamageAmount;
}

void AParentCharacter::AttackCombo()
{
	if (CombatState != ECombatState::ECS_Unoccupied) { return; }

	CombatState = ECombatState::ECS_Attacking;
}

void AParentCharacter::InitializeBaseStats()
{
	if(BaseStatsData)
	{
		const FString ContextString = RowName.ToString();
		BaseStatsStruct = BaseStatsData->FindRow<FBaseStats>(RowName, ContextString, true); 
	}
}

