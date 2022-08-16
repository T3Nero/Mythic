// Fill out your copyright notice in the Description page of Project Settings.


#include "MythicAnimInstance.h"
#include "ParentCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

		FRotator AimRotation = Character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// Checks if player has pressed crouch button and updates animations
		if (Character->GetCrouching())
		{
			bIsCrouching = true;
		}
		else
		{
			bIsCrouching = false;
		}

		if (bIsCrouching)
		{
			Character->SetWeaponDrawn(false);
		}
	}
}

void UMythicAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AParentCharacter>(TryGetPawnOwner());
}