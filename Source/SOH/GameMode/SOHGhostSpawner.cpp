#include "SOHGhostSpawner.h"
#include "SOHGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASOHGhostSpawner::ASOHGhostSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASOHGhostSpawner::BeginPlay()
{
    Super::BeginPlay();

    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (GI)
    {
        // GameInstance에서 Condition이 완료될 때마다 이 함수가 호출됨
        GI->OnConditionCompleted.AddDynamic(this, &ASOHGhostSpawner::OnConditionCompleted);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner: GameInstance 없음!"));
    }
}

void ASOHGhostSpawner::OnConditionCompleted(FGameplayTag CompletedTag)
{
    UE_LOG(LogTemp, Warning, TEXT("Spawner Received Condition: %s"), *CompletedTag.ToString());

    if (CompletedTag == SpawnConditionTag)
    {
        SpawnGhost();
    }

    if (CompletedTag == DespawnConditionTag)
    {
        DespawnGhost();
    }
}

void ASOHGhostSpawner::SpawnGhost()
{
    if (!GhostClass)
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner: GhostClass 지정 안됨!"));
        return;
    }

    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner: TargetActor 없음!"));
        return;
    }

    if (SpawnedGhost)
    {
        UE_LOG(LogTemp, Warning, TEXT("GhostSpawner: 이미 귀신 스폰됨"));
        return;
    }

    FTransform SpawnTransform = TargetActor->GetActorTransform();

    SpawnedGhost = GetWorld()->SpawnActor<ACharacter>(GhostClass, SpawnTransform);

    if (SpawnedGhost)
    {
        UE_LOG(LogTemp, Warning, TEXT("GhostSpawner: 귀신 스폰 성공 (%s)"),
            *SpawnTransform.GetLocation().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner: 귀신 스폰 실패"));
    }
}

void ASOHGhostSpawner::DespawnGhost()
{
    if (!SpawnedGhost)
    {
        UE_LOG(LogTemp, Warning, TEXT("GhostSpawner: 제거할 귀신 없음"));
        return;
    }

    SpawnedGhost->Destroy();
    SpawnedGhost = nullptr;

    UE_LOG(LogTemp, Warning, TEXT("GhostSpawner: 귀신 제거됨"));
}
