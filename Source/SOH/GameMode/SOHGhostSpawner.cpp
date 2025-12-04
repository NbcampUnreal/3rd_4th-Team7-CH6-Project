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

    // 초기 설정 검증
    if (!GhostClass)
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner [%s]: GhostClass가 블루프린트에서 설정되지 않음!"), *GetName());
    }
    
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner [%s]: TargetActor가 블루프린트에서 설정되지 않음!"), *GetName());
    }

    if (!SpawnConditionTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner [%s]: SpawnConditionTag가 설정되지 않음!"), *GetName());
    }

    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (GI)
    {
        GI->OnConditionCompleted.AddDynamic(this, &ASOHGhostSpawner::OnConditionCompleted);
        UE_LOG(LogTemp, Log, TEXT("GhostSpawner [%s]: GameInstance 이벤트 바인딩 성공"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GhostSpawner [%s]: GameInstance 없음!"), *GetName());
    }
}

void ASOHGhostSpawner::OnConditionCompleted(FGameplayTag CompletedTag)
{
    UE_LOG(LogTemp, Warning, TEXT("=== Spawner [%s] Received Condition: %s ==="), 
        *GetName(), *CompletedTag.ToString());
    
    UE_LOG(LogTemp, Warning, TEXT("    SpawnConditionTag: %s"), 
        *SpawnConditionTag.ToString());
    
    UE_LOG(LogTemp, Warning, TEXT("    DespawnConditionTag: %s"), 
        *DespawnConditionTag.ToString());

    // MatchesTagExact 사용 (더 안전한 비교)
    if (CompletedTag.MatchesTagExact(SpawnConditionTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("    -> SpawnConditionTag 일치! SpawnGhost 호출"));
        SpawnGhost();
    }
    else if (CompletedTag.MatchesTagExact(DespawnConditionTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("    -> DespawnConditionTag 일치! DespawnGhost 호출"));
        DespawnGhost();
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("    -> 일치하는 태그 없음"));
    }
}

void ASOHGhostSpawner::SpawnGhost()
{
    UE_LOG(LogTemp, Warning, TEXT("=== SpawnGhost 시작 [%s] ==="), *GetName());

    if (!GhostClass)
    {
        UE_LOG(LogTemp, Error, TEXT("    X GhostClass 지정 안됨!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("    O GhostClass: %s"), *GhostClass->GetName());

    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("    X TargetActor 없음!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("    O TargetActor: %s"), *TargetActor->GetName());

    if (SpawnedGhost && IsValid(SpawnedGhost))
    {
        UE_LOG(LogTemp, Warning, TEXT("    X 이미 귀신 스폰됨 (%s)"), *SpawnedGhost->GetName());
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("    X World가 유효하지 않음!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("    O World 유효"));

    FTransform SpawnTransform = TargetActor->GetActorTransform();
    UE_LOG(LogTemp, Log, TEXT("    스폰 위치: %s"), *SpawnTransform.GetLocation().ToString());

    // SpawnParameters 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = 
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = nullptr;

    SpawnedGhost = World->SpawnActor<ACharacter>(
        GhostClass, 
        SpawnTransform, 
        SpawnParams
    );

    if (SpawnedGhost)
    {
        UE_LOG(LogTemp, Warning, TEXT("    O 귀신 스폰 성공! (%s) at %s"),
            *SpawnedGhost->GetName(),
            *SpawnTransform.GetLocation().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("    X 귀신 스폰 실패!"));
        UE_LOG(LogTemp, Error, TEXT("      - GhostClass: %s"), *GhostClass->GetName());
        UE_LOG(LogTemp, Error, TEXT("      - 위치: %s"), *SpawnTransform.GetLocation().ToString());
        UE_LOG(LogTemp, Error, TEXT("      -> 블루프린트에서 GhostClass 설정 확인 필요!"));
    }
}

void ASOHGhostSpawner::DespawnGhost()
{
    if (!SpawnedGhost || !IsValid(SpawnedGhost))
    {
        UE_LOG(LogTemp, Warning, TEXT("GhostSpawner [%s]: 제거할 귀신 없음"), *GetName());
        SpawnedGhost = nullptr;  // nullptr로 정리
        return;
    }

    FString GhostName = SpawnedGhost->GetName();
    SpawnedGhost->Destroy();
    SpawnedGhost = nullptr;

    UE_LOG(LogTemp, Warning, TEXT("GhostSpawner [%s]: 귀신 제거됨 (%s)"), *GetName(), *GhostName);
}