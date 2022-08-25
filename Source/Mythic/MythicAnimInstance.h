// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MythicAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYTHIC_API UMythicAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UMythicAnimInstance();

protected:

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class AParentCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bIsMoving;

public:

	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;
};
