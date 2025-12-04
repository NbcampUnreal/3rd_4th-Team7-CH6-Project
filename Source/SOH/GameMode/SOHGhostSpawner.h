#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Engine/TargetPoint.h"
#include "SOHGhostSpawner.generated.h"

class USOHGameInstance;
class ACharacter;

UCLASS()
class SOH_API ASOHGhostSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASOHGhostSpawner();

protected:
	virtual void BeginPlay() override;

	// --- 스폰 조건 태그 ---
	UPROPERTY(EditAnywhere, Category="Spawner")
	FGameplayTag SpawnConditionTag;

	// --- 제거 조건 태그 ---
	UPROPERTY(EditAnywhere, Category="Spawner")
	FGameplayTag DespawnConditionTag;

	// --- 스폰할 귀신 클래스 ---
	UPROPERTY(EditAnywhere, Category="Spawner")
	TSubclassOf<ACharacter> GhostClass;

	// --- 스폰 위치 Target Actor ---
	UPROPERTY(EditAnywhere, Category="Spawner")
	ATargetPoint* TargetActor;

	// --- 현재 스폰된 귀신 ---
	UPROPERTY()
	ACharacter* SpawnedGhost;

	// --- GameInstance 이벤트를 받는 함수 ---
	UFUNCTION()
	void OnConditionCompleted(FGameplayTag CompletedTag);

	// --- 스폰 기능 ---
	void SpawnGhost();

	// --- 제거 기능 ---
	void DespawnGhost();
};
