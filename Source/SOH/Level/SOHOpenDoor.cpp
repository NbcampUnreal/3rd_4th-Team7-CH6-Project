#include "SOHOpenDoor.h"
#include "Item/SOHLockActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"

ASOHOpenDoor::ASOHOpenDoor()
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

void ASOHOpenDoor::BeginPlay()
{
	Super::BeginPlay();
}

void ASOHOpenDoor::Interact_Implementation(AActor* Caller)
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

	if (bLocked)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			USOHMessageManager* MsgManager = PC->FindComponentByClass<USOHMessageManager>();
			if (MsgManager)
			{
				MsgManager->ShowMessageText(FText::FromString(TEXT("문이 잠겨 있습니다.")), 1.5f);
			}
		}

		return;
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

void ASOHOpenDoor::NotifyDoorMoveStarted()
{
	bIsMoving = true;
}

void ASOHOpenDoor::NotifyDoorMoveFinished(bool bNowOpen)
{
	bIsOpen = bNowOpen;
	bIsMoving = false;
}

void ASOHOpenDoor::UnlockOpenDoor(AActor* Caller)
{
	if (!bLocked)
	{
		return;
	}

	bLocked = false;
}