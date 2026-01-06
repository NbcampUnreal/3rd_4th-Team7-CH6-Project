#include "SOHOpenDoor.h"
#include "Item/SOHLockActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"
#include "GameFramework/Character.h"
#include "AI/SOHNoiseSound.h"

ASOHOpenDoor::ASOHOpenDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(Root);

	if (InteractionRange)
		InteractionRange->SetupAttachment(Root);

	if (UIAnchor)
	{
		UIAnchor->SetupAttachment(DoorFrame);
		UIAnchor->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	}

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
		ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (PlayerChar)
		{
			if (USOHMessageManager* MsgMgr = PlayerChar->FindComponentByClass<USOHMessageManager>())
			{
				MsgMgr->ShowMessageText(
					FText::FromString(TEXT("문이 잠겨 있다.")),
					1.5f
				);
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

			if (bUseNoiseEvent)
			{
				USOHNoiseSound::PlaySoundWithNoise(
					this,
					OpenSound,
					GetActorLocation(),
					NoiseLoudness,
					NoiseMaxRange,
					NoiseTag,
					Caller
				);
			}
		}
		BP_OpenDoor(Caller);
	}
	else
	{
		if (CloseSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());

			if (bUseNoiseEvent)
			{
				USOHNoiseSound::PlaySoundWithNoise(
					this,
					CloseSound,
					GetActorLocation(),
					NoiseLoudness,
					NoiseMaxRange,
					NoiseTag,
					Caller
				);
			}
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

	if (!bIsOpen && !bIsMoving)
	{
		bIsMoving = true;

		if (OpenSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}

		BP_OpenDoor(Caller);

		UE_LOG(LogTemp, Warning, TEXT("[Door] %s auto-opening"), *GetName());
	}
}