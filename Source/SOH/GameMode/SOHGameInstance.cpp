#include "SOHGameInstance.h"
#include "SOHSaveGame.h"
#include "Engine/DataTable.h"
#include "Character/SOHPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

USOHGameInstance::USOHGameInstance()
{
	//초기값
}

void USOHGameInstance::CompleteCondition(FGameplayTag ConditionTag)
{
	// None 태그 방어
	if (!ConditionTag.IsValid() || ConditionTag.ToString() == "None")
	{
		UE_LOG(LogTemp, Error, TEXT("[Condition] None 태그로 호출됨! 무시됨"));
		return;
	}

	if (CompletedConditions.HasTagExact(ConditionTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Condition] 이미 완료됨: %s (재브로드캐스트)"), 
			*ConditionTag.ToString());
        
		// 이미 완료된 조건도 Broadcast!
		OnConditionCompleted.Broadcast(ConditionTag);
		return;
	}

	CompletedConditions.AddTag(ConditionTag);
	UE_LOG(LogTemp, Warning, TEXT("[Condition] 완료: %s"), *ConditionTag.ToString());

	OnConditionCompleted.Broadcast(ConditionTag);
    
	if (IsStageCompleted(CurrentStage))
	{
		AdvanceStage();
	}
}
bool USOHGameInstance::HasCondition(FGameplayTag ConditionTag) const
{
	return CompletedConditions.HasTagExact(ConditionTag);
}

bool USOHGameInstance::IsStageCompleted(int32 StageID)
{
	if (!StageDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[Stage] StageDataTable 없음!"));
		return false;
	}

	FName RowName(*FString::FromInt(StageID));
	FSOHStageData* Row = StageDataTable->FindRow<FSOHStageData>(RowName, TEXT(""));

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Stage] Row %d 없음!"), StageID);
		return false;
	}

	return CompletedConditions.HasAll(Row->RequiredConditions);
}

void USOHGameInstance::AdvanceStage()
{
	UE_LOG(LogTemp, Warning, TEXT("==== Stage %d 완료! 다음 Stage로 이동 ===="), CurrentStage);
	CurrentStage++;
}

void USOHGameInstance::SaveGameData()
{
	USOHSaveGame* Save = Cast<USOHSaveGame>(
		UGameplayStatics::CreateSaveGameObject(USOHSaveGame::StaticClass())
	);

	Save->SaveLevelName = FName(*GetWorld()->GetMapName());
	
	Save->SavedStage = CurrentStage;
	Save->SavedConditions = CompletedConditions;

	// 플레이어 정보
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (Player)
	{
		Save->PlayerTransform = Player->GetActorTransform();

		// 체력 스테미너는 정현 PlayerCharacter에서 Get함수 만들면 읽어올 수 있음
		ASOHPlayerCharacter* P = Cast<ASOHPlayerCharacter>(Player);
		if (P)
		{
			Save->SavedHealth = P->GetHealth();
			Save->SavedStamina = P->GetStamina();
		}
	}

	UGameplayStatics::SaveGameToSlot(Save, TEXT("SaveSlot1"), 0);

	UE_LOG(LogTemp, Warning, TEXT("[SAVE] Game saved successfully."));
}

bool USOHGameInstance::LoadGameData()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot1"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LOAD] Save file not found."));
		return false;
	}

	USOHSaveGame* Save = Cast<USOHSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot1"), 0)
	);
	if (!Save) return false;

	// Stage 복원
	CurrentStage = Save->SavedStage;
	CompletedConditions = Save->SavedConditions;

	// 레벨 로드
	UGameplayStatics::OpenLevel(this, Save->SaveLevelName);

	// 플레이어 위치/체력은 "레벨 로드 후" BeginPlay에서 복원할 것
	LoadedPlayerTransform = Save->PlayerTransform;
	LoadedHealth = Save->SavedHealth;
	LoadedStamina = Save->SavedStamina;

	UE_LOG(LogTemp, Warning, TEXT("[LOAD] Game loaded successfully."));
	return true;
}

