// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mythic/InteractInterface.h"

#include "ParentItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Null UMETA(DisplayName = "Null"),
	EIT_Potion UMETA(DisplayName = "Potion"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Armour UMETA(DisplayName = "Armour"),
	EIT_Accessory UMETA(DisplayName = "Accessory"),
	EIT_QuestItem UMETA(DisplayName = "Quest Item"),

	EIT_MAX
};

USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

};

UCLASS()
class MYTHIC_API AParentItem : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParentItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnShowItemWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHideItemWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Components", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickUpWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* PickUpCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

	// stores information about each item into a struct (edit via Blueprints)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FItemInfo ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName WeaponDrawnSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName WeaponSheathedSocket;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SheatheWeapon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when player presses Interact button & is near the item (Picks up the Item)
	// virtual void InteractPure() override; // c++ only, overriden from InteractInterface
	
	// Called when player presses Interact button & is near the item (Picks up the Item)
	// Must include " _Implementation " when declaring Blueprint native Events (Functions)
	// overriden from InteractInterface
	void Interact_Implementation() override; // Declared here but will be used in Blueprints as an Interact Event

	FORCEINLINE UWidgetComponent* GetPickUpWidget() const { return PickUpWidget; }
	FORCEINLINE USphereComponent* GetPickUpCollision() const { return PickUpCollision; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE FItemInfo GetItemInfo() const { return ItemInfo; }
	FORCEINLINE FName GetWeaponDrawnSocket() const { return WeaponDrawnSocket; }
	FORCEINLINE FName GetWeaponSheathedSocket() const { return WeaponSheathedSocket; }
	FORCEINLINE UAnimMontage* GetDrawWeaponMontage() const { return DrawWeapon; }
	FORCEINLINE UAnimMontage* GetSheatheWeaponMontage() const { return SheatheWeapon; }

};
