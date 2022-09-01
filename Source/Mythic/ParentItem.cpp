// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentItem.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"

// Sets default values
AParentItem::AParentItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	RootComponent = ItemMesh;

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUp Widget"));
	PickUpWidget->SetupAttachment(ItemMesh);

	PickUpCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickUp Collision"));
	PickUpCollision->SetupAttachment(ItemMesh);

	ItemInfo.ItemType = EItemType::EIT_Null;
}

// Called when the game starts or when spawned
void AParentItem::BeginPlay()
{
	Super::BeginPlay();

	

	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}

	PickUpCollision->OnComponentBeginOverlap.AddDynamic(this, &AParentItem::OnShowItemWidget);
	PickUpCollision->OnComponentEndOverlap.AddDynamic(this, &AParentItem::OnHideItemWidget);
}


// Called every frame
void AParentItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Must be defined but can be left empty if using the Event in Blueprints
void AParentItem::Interact_Implementation()
{
	if (Player)
	{
		Player->GetItemPickedUp(this);
	}
}

void AParentItem::OnShowItemWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			PickUpWidget->SetVisibility(true);
		}
	}
}

void AParentItem::OnHideItemWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			PickUpWidget->SetVisibility(false);
		}
	}
}

