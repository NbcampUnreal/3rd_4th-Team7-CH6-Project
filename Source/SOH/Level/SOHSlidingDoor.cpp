#include "SOHSlidingDoor.h"
#include "Item/SOHLockActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASOHSlidingDoor::ASOHSlidingDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(Root);

	if (DoorFrame)
	{
		DoorFrame->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void ASOHSlidingDoor::BeginPlay()
{
	Super::BeginPlay();
}

void ASOHSlidingDoor::Interact_Implementation(AActor* Caller)
{
	if (bIsMoving) return;

	if (bLocked)
	{
		if (LinkedLockActor)
		{
			if (IsValid(LinkedLockActor))
			{
				BP_OnLocked(Caller);
				return;
			}
			else
			{
				bLocked = false;
			}
		}
		else
		{
			BP_OnLocked(Caller);
			return;
		}
	}

	bIsMoving = true;

	if (!bIsOpen)
	{
		if (OpenSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}

		BP_OpenDoor(Caller);
	}
	else
	{
		if (CloseSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
		}

		BP_CloseDoor(Caller);
	}
}

void ASOHSlidingDoor::NotifyDoorMoveStarted()
{
	bIsMoving = true;
}

void ASOHSlidingDoor::NotifyDoorMoveFinished(bool bNowOpen)
{
	bIsOpen = bNowOpen;
	bIsMoving = false;
}

void ASOHSlidingDoor::UnlockSlidingDoor(AActor* Caller)
{
	if (!bLocked)
	{
		return;
	}

	bLocked = false;

	if (!bIsOpen)
	{
		if (OpenSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}

		BP_OpenDoor(Caller);
	}
}