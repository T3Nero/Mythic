// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework//SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

APlayerCharacter::APlayerCharacter() :
	MouseTurnSensitivity(1.f),
	MouseLookSensitivity(1.f),
	GamepadTurnSensitivity(90.f),
	GamepadLookSensitivity(90.f)
{

	// Creates a Camera Boom / Spring Arm (Sets Distance from Player to Camera)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // Distance away from Player
	CameraBoom->bUsePawnControlRotation = true; // Rotate spring arm based on player movement
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 100.f); // Offsets position of camera behind player

	// Creates Follow Camera behind Player
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Sets the follow camera to be a child of Camera Boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to spring arm

	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::MoveForwardBackward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Calculate which X Axis Direction to move in based on Controllers Yaw Rotation
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);

		if (Value > 0)
		{
			IsMovingForward = true;
		}
		else if (Value < 0)
		{
			IsMovingBackward = true;
		}
	}
	else if (Value == 0)
	{
		IsMovingForward = false;
		IsMovingBackward = false;
	}
}

void APlayerCharacter::MoveRightLeft(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Calculate which Y Axis Direction to move in based on Controllers Yaw Rotation
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);

		if (Value > 0)
		{
			IsMovingRight = true;
		}
		else if (Value < 0)
		{
			IsMovingLeft = true;
		}
	}
	else if (Value == 0)
	{
		IsMovingLeft = false;
		IsMovingRight = false;
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * MouseTurnSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * GamepadTurnSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value * MouseLookSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookAtRate(float Value)
{
	AddControllerPitchInput(Value * GamepadLookSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::Jump()
{
	if (GetCrouching())
	{
		SetCrouching(false);
		GetCharacterMovement()->MaxWalkSpeed = GetBaseMovementSpeed();
	}
	else
	{
		ACharacter::Jump();
	}
}

void APlayerCharacter::ZoomCamera(float Value)
{
	if (CameraBoom && Value != 0)
	{
		float CurrentArmLength = CameraBoom->TargetArmLength;
		float ZoomValue = CurrentArmLength - Value * 100.f;
		ZoomValue = FMath::Clamp(ZoomValue, 250.f, 1000.f);
		CameraBoom->TargetArmLength = FMath::FInterpTo(CurrentArmLength, ZoomValue, UGameplayStatics::GetWorldDeltaSeconds(this), 20.f);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward/Backward"), this, &APlayerCharacter::MoveForwardBackward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight/Left"), this, &APlayerCharacter::MoveRightLeft);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("TurnAtRate"), this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookAtRate"), this, &APlayerCharacter::LookAtRate);
	PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &APlayerCharacter::ZoomCamera);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AParentCharacter::Crouching);
	PlayerInputComponent->BindAction(TEXT("Dodge"), EInputEvent::IE_Pressed, this, &AParentCharacter::Dodge);
}
