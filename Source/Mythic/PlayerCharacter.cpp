// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework//SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "ParentItem.h"
#include "Weapon.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"

APlayerCharacter::APlayerCharacter() :
	MouseTurnSensitivity(20.f),
	MouseLookSensitivity(20.f),
	GamepadTurnSensitivity(70.f),
	GamepadLookSensitivity(70.f)
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
	if (GetCombatState() == ECombatState::ECS_Unoccupied || GetCombatState() == ECombatState::ECS_Dodging)
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
}

void APlayerCharacter::MoveRightLeft(float Value)
{
	if (GetCombatState() == ECombatState::ECS_Unoccupied || GetCombatState() == ECombatState::ECS_Dodging)
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

void APlayerCharacter::Crouching()
{
	if (GetCombatState() != ECombatState::ECS_Unoccupied) { return; }

	if (GetCanStand())
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			SetCrouching(!GetCrouching());
		}
	}
}

void APlayerCharacter::Jump()
{
	if (GetCrouching())
	{
		if (GetCanStand())
		{
			SetCrouching(false);
			GetCharacterMovement()->MaxWalkSpeed = GetBaseMovementSpeed();
		}
	}
	else
	{
		if (GetCombatState() != ECombatState::ECS_Unoccupied) { return; }
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

void APlayerCharacter::InteractButtonPressed()
{
	if (GetCombatState() != ECombatState::ECS_Unoccupied) { return; }
	if (GetEquippedWeapon()) { return; }

	FVector Start = GetActorLocation();
	FVector End = GetActorLocation();
	End.Z -= 50;

	FHitResult HitActor;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery(ECC_WorldDynamic));
	
	// Does a single sphere trace around the player when interact button is pressed 
	// returns true if an actor is hit

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), Start, End, 30.f, ObjectTypes, false,
		ActorsToIgnore, EDrawDebugTrace::None, HitActor, true,
		FLinearColor::Blue, FLinearColor::Green, 5.f);

	if (bHit)
	{
		// Checks to see if the HitActor implements the InteractInterface class
		if (HitActor.GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			// Creates a reference to InteractInterface
			IInteractInterface* InterfaceReference = Cast<IInteractInterface>(HitActor.GetActor());
			if (InterfaceReference)
			{
				// Calls Interact function on any HitActor which implements InteractInterface
				InterfaceReference->Execute_Interact(HitActor.GetActor());
			}
		}
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (Weapon->CheckWeaponRequirements(this))
	{
		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(FName(Weapon->GetWeaponDrawnSocket()));
		if (WeaponSocket)
		{
			WeaponSocket->AttachActor(Weapon, GetMesh());
		}
		SetEquippedWeapon(Weapon);
		SetWeaponSpeed(Weapon->GetWeaponSpeed());
		SetWeaponDrawn(true);
		SetStrafing(true);
		ToggleStrafing();
		UpdateAttributes(true);
	}
}

void APlayerCharacter::AttackButtonPressed()
{
	// Attack Combo
	if (GetCrouching() || GetCharacterMovement()->IsFalling()) { return; }
	if (GetCombatState() == ECombatState::ECS_Dodging) { return; }

	if (GetEquippedWeapon())
	{
		if (!GetWeaponDrawn())
		{
			PlayDrawSheatheWeaponMontage();
		}
		else
		{
			AttackCombo();
		}
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
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Crouching);
	PlayerInputComponent->BindAction(TEXT("Dodge"), EInputEvent::IE_Pressed, this, &APlayerCharacter::DodgeButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &APlayerCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Draw/SheatheWeapon"), EInputEvent::IE_Pressed, this, &AParentCharacter::PlayDrawSheatheWeaponMontage);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &APlayerCharacter::AttackButtonPressed);
}

void APlayerCharacter::GetItemPickedUp(AParentItem* Item)
{
	if (Item->GetItemType() == EItemType::EIT_Weapon)
	{
		AWeapon* Weapon = Cast<AWeapon>(Item);
		EquipWeapon(Weapon);
	}
}

void APlayerCharacter::AttackCombo()
{
	if (GetCanAttack())
	{
		UAnimMontage* Attack = GetEquippedWeapon()->GetAttackMontage();

		if (GetWeaponDrawn())
		{
			if (Attack)
			{
				FName SectionName;
				switch (GetComboIndex())
				{
				case 0:
					SectionName = "Attack01";
					break;
				case 1:
					SectionName = "Attack02";
					break;
				case 2:
					SectionName = "Attack03";
					break;
				case 3:
					SectionName = "Attack04";
					break;
				default:
					SectionName = "Attack01";
					break;
				}
				SetCombatState(ECombatState::ECS_Attacking);
				SetCanAttack(false);
				SetMontageToPlay(Attack, SectionName);
			}
		}
	}
}

void APlayerCharacter::DodgeButtonPressed()
{
	if (GetCrouching()) { return; }

	if (GetCombatState() == ECombatState::ECS_Unoccupied || GetCombatState() == ECombatState::ECS_Attacking)
	{
		FName SectionName = "Forward";

		if (GetStrafing())
		{
			if (IsMovingForward)
			{
				SectionName = "Forward";
			}
			if (IsMovingBackward)
			{
				SectionName = "Backward";
			}
			if (IsMovingLeft)
			{
				SectionName = "Left";
			}
			if (IsMovingRight)
			{
				SectionName = "Right";
			}
			if (IsMovingRight && IsMovingForward)
			{
				SectionName = "ForwardRight";
			}
			if (IsMovingLeft && IsMovingForward)
			{
				SectionName = "ForwardLeft";
			}
			if (IsMovingLeft && IsMovingBackward)
			{
				SectionName = "BackwardLeft";
			}
			if (IsMovingRight && IsMovingBackward)
			{
				SectionName = "BackwardRight";
			}
		}

		if(BaseStatsStruct->Adrenaline >= 10)
		{
			SetMontageToPlay(DodgeMontage, SectionName);
			BaseStatsStruct->Adrenaline = BaseStatsStruct->Adrenaline -= 10;
		}
		else
		{
			return;
		}

		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
		SetCombatState(ECombatState::ECS_Dodging);
	}
}

void APlayerCharacter::SetMontageToPlay(UAnimMontage* Montage, FName Section) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(Section);

	// Sets play rate of attack combo based on equipped weapon speed
	if (GetCombatState() == ECombatState::ECS_Attacking)
	{
		AnimInstance->Montage_SetPlayRate(Montage, GetWeaponSpeed());
	}
}
