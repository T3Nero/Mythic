// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"

AAICharacter::AAICharacter() :
	HitNumberDestroyTime(1.5f)
{
}

void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();
}

void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon(SpawnWeapon());
}

void AAICharacter::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(
		HitNumberTimer, 
		HitNumberDelegate, 
		HitNumberDestroyTime, 
		false);
}

void AAICharacter::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AAICharacter::UpdateHitNumbers()
{
	for (const auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{HitPair.Key};
		const FVector Location{HitPair.Value};
		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(), 
			Location, 
			ScreenPosition);

		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

AWeapon* AAICharacter::SpawnWeapon() const
{
	if(WeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	}

	return nullptr;
}

void AAICharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(WeaponToEquip)
	{
		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(FName(WeaponToEquip->GetWeaponDrawnSocket()));
		if(WeaponSocket)
		{
			WeaponSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		SetEquippedWeapon(WeaponToEquip);
		WeaponToEquip->GetPickUpCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetWeaponSpeed(WeaponToEquip->GetWeaponSpeed());
		SetWeaponDrawn(true);
	}
}
