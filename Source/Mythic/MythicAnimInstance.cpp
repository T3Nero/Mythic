// Fill out your copyright notice in the Description page of Project Settings.


#include "MythicAnimInstance.h"
#include "ParentCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"

UMythicAnimInstance::UMythicAnimInstance() :
	Speed(0.f),
	Direction(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	bIsCrouching(false)
{

}


void UMythicAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Character == nullptr)
	{
		Character = Cast<AParentCharacter>(TryGetPawnOwner());
	}
	if (Character)
	{
		bIsInAir = Character->GetCharacterMovement()->IsFalling();

		// Sets Speed based on Players current Velocity
		FVector Velocity{ Character->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Checks if Character is currently accelerating
		if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		if (Speed > 3 && bIsAccelerating)
		{
			bIsMoving = true;
		}
		else
		{
			bIsMoving = false;
		}

		// Sets Direction based on characters AimRotation and Movement Rotation (Characters X Velocity)
		// ** Used for Strafing purposes **
		FRotator AimRotation = Character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());

		if (Character->GetStrafing())
		{
			Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		}
		else
		{
			Direction = 0;
		}

		// Checks if player/npc is crouching and updates animations / speed
		if (Character->GetCrouching())
		{
			bIsCrouching = true;
			Character->GetCharacterMovement()->MaxWalkSpeed = 300.f;
		}
		else
		{
			bIsCrouching = false;
			Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetBaseMovementSpeed();
		}

	}
}

void UMythicAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AParentCharacter>(TryGetPawnOwner());
}