#include "SOHOpenDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

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
		BP_OnLocked(Caller);
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
