#include "SOHSlidingDoor.h"
#include "Item/SOHLockActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"

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
		DoorFrame->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
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

void ASOHSlidingDoor::NotifyDoorMoveStarted()
{
	bIsMoving = true;
}

void ASOHSlidingDoor::NotifyDoorMoveFinished(bool bNowOpen)
{
	bIsOpen = bNowOpen;
	bIsMoving = false;

	DoorFrame->SetCollisionResponseToChannel(ECC_GameTraceChannel1, bIsOpen ? ECR_Ignore : ECR_Block);
}

void ASOHSlidingDoor::UnlockSlidingDoor(AActor* Caller)
{
	if (!bLocked)
	{
		return;
	}

	bLocked = false;
}