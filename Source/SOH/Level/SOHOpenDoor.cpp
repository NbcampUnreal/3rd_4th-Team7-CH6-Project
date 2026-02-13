#include "SOHOpenDoor.h"
#include "Item/SOHLockActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"
#include "GameFramework/Character.h"
#include "GameMode/SOHSaveGame.h"
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

	// ✅ 핵심: "결정 순간"에 목표 상태를 먼저 반영
	// (저장/로드는 이 목표 상태를 기준으로 움직이게 됨)
	const bool bWillOpen = !bIsOpen;
	bIsOpen = bWillOpen;
	bIsMoving = true;

	if (bWillOpen)
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
	// bIsOpen은 이미 "목표 상태"로 바뀌어 있을 가능성이 높지만,
	// 최종 동기화로 맞춰주는 건 안전함 (네트워크/블렌딩 등 예외 대비)
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

	// ✅ 잠금 해제 후 자동 오픈이 결정되는 순간 목표 상태를 먼저 true로
	if (!bIsOpen && !bIsMoving)
	{
		bIsOpen = true;     // ✅ 핵심: "열릴 예정" 상태를 먼저 반영
		bIsMoving = true;

		if (OpenSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}

		BP_OpenDoor(Caller);

		UE_LOG(LogTemp, Warning, TEXT("[Door] %s auto-opening"), *GetName());
	}
}

void ASOHOpenDoor::SaveState_Implementation(USOHSaveGame* SaveData)
{
	if (!SaveData || WorldStateID.IsNone()) return;

	// ✅ 핵심: 이동 중이어도 저장 스킵하지 않음
	FWorldStateData& Data = SaveData->WorldStateMap.FindOrAdd(WorldStateID);
	Data.bIsLocked = bLocked;
	Data.bIsOpen = bIsOpen;
}

void ASOHOpenDoor::LoadState_Implementation(USOHSaveGame* SaveData)
{
	UE_LOG(LogTemp, Error, TEXT("🔥 Door LoadState CALLED: %s"), *GetName());

	if (!SaveData || WorldStateID.IsNone()) return;

	if (FWorldStateData* Data = SaveData->WorldStateMap.Find(WorldStateID))
	{
		bLocked = Data->bIsLocked;
		bIsOpen = Data->bIsOpen;
		bIsMoving = false;

		if (DoorFrame)
		{
			if (bIsOpen)
			{
				DoorFrame->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			}
			else
			{
			}
		}
	}
}