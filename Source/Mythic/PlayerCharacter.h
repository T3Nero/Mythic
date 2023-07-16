// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MYTHIC_API APlayerCharacter : public AParentCharacter
{
	GENERATED_BODY()
	
public:

	APlayerCharacter();

protected:

	// Input for moving Forward/Backward
	void MoveForwardBackward(float Value);

	// Input for moving Right/Left
	void MoveRightLeft(float Value);

	// Rotate Controller X-Axis using Mouse * Value for Sensitivity
	void Turn(float Value);

	// Rotate Controller Y-Axis using the Mouse * Value for Sensitivity
	void LookUp(float Value);

	/*
	*  Called via input to turn at a given rate
	*  @param Value	 This is a normalized rate, i.e 1.0 means 100% desired turn rate
	*/
	// Gamepad Input
	void TurnAtRate(float Value);

	/*
	*  Called via input to look up/down at a given rate
	*  @param Value	 This is a normalized rate, i.e 1.0 means 100% desired look at rate
	*/
	// Gamepad Input
	void LookAtRate(float Value);

	void Crouching();

	// Controls Jumping & Dodging (Same input keys)
	virtual void Jump() override;

	// Lerps camera boom in/out when inputing mouse wheel axis forward/backward
	void ZoomCamera(float Value);

	// Press T or X (Xbox Gamepad) to Interact (PickUp item etc.)
	void InteractButtonPressed();

	void EquipWeapon(class AWeapon* Weapon);

	void AttackButtonPressed();

	// Overridden from parent class (parents AttackCombo() is for AI Attack)
	virtual void AttackCombo();

	void DodgeButtonPressed();

	void SetMontageToPlay(UAnimMontage* Montage, FName Section) const;

public:

	virtual void Tick(float DeltaTime);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// Sets how quickly the player turns left/right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float MouseTurnSensitivity;

	// Sets how quickly the player looks up/down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float MouseLookSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float GamepadTurnSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float GamepadLookSensitivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// Used to know which direction the player should dodge in
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool IsMovingForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool IsMovingBackward;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool IsMovingRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime", meta = (AllowPrivateAccess = "true"))
		bool IsMovingLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DodgeMontage;



public:

	void GetItemPickedUp(class AParentItem* Item);

};
