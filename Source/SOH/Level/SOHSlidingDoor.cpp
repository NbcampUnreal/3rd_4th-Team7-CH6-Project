#include "SOHSlidingDoor.h"
#include "Item/SOHLockActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"
#include "GameFramework/Character.h"
#include "GameMode/SOHSaveGame.h"
#include "AI/SOHNoiseSound.h"

ASOHSlidingDoor::ASOHSlidingDoor()
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

			//if (bUseNoiseEvent)
			//{
			//	USOHNoiseSound::PlaySoundWithNoise(
			//		this,
			//		OpenSound,
			//		GetActorLocation(),
			//		NoiseLoudness,
			//		NoiseMaxRange,
			//		NoiseTag,
			//		Caller
			//	);
			//}
		}

		BP_OpenDoor(Caller);
	}
	else
	{
		if (CloseSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());

			//if (bUseNoiseEvent)
			//{
			//	USOHNoiseSound::PlaySoundWithNoise(
			//		this,
			//		CloseSound,
			//		GetActorLocation(),
			//		NoiseLoudness,
			//		NoiseMaxRange,
			//		NoiseTag,
			//		Caller
			//	);
			//}
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

void ASOHSlidingDoor::LockAndCloseDoor(AActor* Caller)
{
	if (bLocked) return;

	bLocked = true;

	if (!bIsOpen || bIsMoving)
	{
		BP_OnLocked(Caller);
		return;
	}

	bIsMoving = true;

	//if (CloseSound)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
	//}

	BP_CloseDoor(Caller);
}

void ASOHSlidingDoor::SaveState_Implementation(USOHSaveGame* SaveData)
{
	if (!SaveData || WorldStateID.IsNone()) return;

	// 🔥 문 이동 중이면 저장하지 않음
	if (bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Save] Door %s is moving. Skip save."), *WorldStateID.ToString());
		return;
	}

	FWorldStateData& Data = SaveData->WorldStateMap.FindOrAdd(WorldStateID);
	Data.bIsLocked = bLocked;
	Data.bIsOpen = bIsOpen;

	UE_LOG(LogTemp, Log, TEXT("[Save] Door %s saved. Open=%d"), *WorldStateID.ToString(), bIsOpen);
}

void ASOHSlidingDoor::LoadState_Implementation(USOHSaveGame* SaveData)
{
	UE_LOG(LogTemp, Error, TEXT("🔥 Door LoadState CALLED: %s"), *GetName());

	if (!SaveData || WorldStateID.IsNone()) return;

	if (FWorldStateData* Data = SaveData->WorldStateMap.Find(WorldStateID))
	{
		// ✅ 1) 잠금 상태는 항상 복원 (핵심!)
		bLocked = Data->bIsLocked;

		// 이동 상태는 로드 시 정지시키는 게 안전
		bIsMoving = false;

		// ✅ 2) 열림/닫힘 상태 복원
		bIsOpen = Data->bIsOpen;

		if (bIsOpen)
		{
			// 충돌 제거 (열림 상태일 때만)
			DoorFrame->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

			// BP에서 "강제로 열린 상태" 세팅 (즉시 세팅용이면 더 좋음)
			BP_OpenDoor(nullptr);
		}
		else
		{
			// 닫힌 상태면 여기서 굳이 Close 애니메이션 돌리지 말고,
			// 필요하면 "즉시 닫힘 세팅" BP/함수로 위치만 맞춰주는 걸 추천.
			// BP_CloseDoor(nullptr); // ❌ 로드 때 타임라인 돌리면 꼬일 수 있음
		}

		UE_LOG(LogTemp, Warning, TEXT("[LOAD][Door] %s Loaded. Locked=%d Open=%d"),
			*WorldStateID.ToString(), bLocked ? 1 : 0, bIsOpen ? 1 : 0);
	}
}


