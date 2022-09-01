// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ParentItem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AParentCharacter::AParentCharacter() :
	BaseMovementSpeed(600.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	bCrouching(false),
	bStrafing(false),
	bCanStand(true),
	ComboIndex(0),
	bCanAttack(true),
	WeaponSpeed(1.f)

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

	// Sets a base movement speed so it can be reset if characters speed is changed through being slowed etc.
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	// Called in beginplay for testing purposes (will call on blueprints when adding TAB targetenemy input)
	ToggleStrafing();
}

// Called every frame
void AParentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	InterpCapsuleHalfHeight(DeltaTime);
}

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

void AParentCharacter::SetUnoccupied()
{
	// Resets Attack
	ComboIndex = 0;
	bCanAttack = true;

	CombatState = ECombatState::ECS_Unoccupied;
}

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

void AParentCharacter::PlayDrawSheatheWeaponMontage()
{
	if (EquippedWeapon == nullptr) { return; }
	if (bCrouching || GetCharacterMovement()->IsFalling()) { return; }
	if (CombatState != ECombatState::ECS_Unoccupied) { return; }

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{

		if (bWeaponDrawn)
		{
			if (GetEquippedWeapon()->GetSheatheWeaponMontage())
			{
				AnimInstance->Montage_Play(GetEquippedWeapon()->GetSheatheWeaponMontage());
				CombatState = ECombatState::ECS_SheatheWeapon;
			}
		}
		else
		{
			if (GetEquippedWeapon()->GetDrawWeaponMontage())
			{
				AnimInstance->Montage_Play(GetEquippedWeapon()->GetDrawWeaponMontage());
				CombatState = ECombatState::ECS_DrawWeapon;
			}
		}

	}
}

void AParentCharacter::AttackCombo()
{
	if (CombatState != ECombatState::ECS_Unoccupied) { return; }

	CombatState = ECombatState::ECS_Attacking;
}

