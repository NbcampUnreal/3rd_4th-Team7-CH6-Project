#include "SOHAIMonsterBTService.h"
#include "SOHAIMonsterController.h"
#include "SOHAIMonster.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

USOHAIMonsterBTService::USOHAIMonsterBTService()
{
	NodeName = TEXT("Update Patrol Or Keep Target");
	Interval = 0.3f;
	RandomDeviation = 0.05f;
}

void USOHAIMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* MonsterController = OwnerComp.GetAIOwner();
    if (!MonsterController) return;

    UWorld* World = MonsterController->GetWorld();
    if (!World) return;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    ASOHAIMonster* Monster = Cast<ASOHAIMonster>(MonsterController->GetPawn());
    if (!Monster) return;

    const bool bPlayerInRange = BB->GetValueAsBool(TEXT("PlayerInRange"));
    AActor* PlayerActor = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));

    bool bInAttackRange = false;
    if (PlayerActor)
    {
        const float DistSq = FVector::DistSquared(PlayerActor->GetActorLocation(), Monster->GetActorLocation());
        bInAttackRange = (DistSq <= FMath::Square(Monster->AttackRange));
    }
    BB->SetValueAsBool(TEXT("AttackRange"), bInAttackRange);

    const float Now = World->GetTimeSeconds();
    const float Until = BB->GetValueAsFloat(TEXT("SearchUntilTime"));
    if (Until > 0.f && Now >= Until)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVICE] Expired Search → LastKnownLocation CLEAR, Now=%.2f, Until=%.2f"), Now, Until);
        BB->ClearValue(TEXT("LastKnownLocation"));
        BB->ClearValue(TEXT("SearchPoint"));
        BB->ClearValue(TEXT("SearchUntilTime"));
        BB->SetValueAsBool(TEXT("IsSearching"), false);
        MonsterController->ClearFocus(EAIFocusPriority::Gameplay);
    }

    bool bPathFail = false;
    if (bPlayerInRange && PlayerActor)
    {
        if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
        {
            if (const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate))
            {
                FPathFindingQuery Query(MonsterController, *NavData, Monster->GetActorLocation(), PlayerActor->GetActorLocation());
                const FPathFindingResult Result = NavSys->FindPathSync(Query);
                bPathFail = (!Result.IsSuccessful() || !Result.Path.IsValid() || Result.IsPartial());
            }
        }
    }

    const bool bPrevPathFail = BB->GetValueAsBool(TEXT("PathFailing"));
    if (bPathFail != bPrevPathFail)
        BB->SetValueAsBool(TEXT("PathFailing"), bPathFail);

    if (bPlayerInRange && PlayerActor && bPathFail && !bPrevPathFail)
    {
        const bool  bHasLK = BB->IsVectorValueSet(TEXT("LastKnownLocation"));
        const float UntilExisting = BB->GetValueAsFloat(TEXT("SearchUntilTime"));
        const bool  bTimerActive = (UntilExisting > Now);

        if (!(bHasLK && bTimerActive))
        {
            const FVector NewLK = PlayerActor->GetActorLocation();
            BB->SetValueAsVector(TEXT("LastKnownLocation"), NewLK);
            BB->SetValueAsFloat(TEXT("SearchUntilTime"), Now + 10.f);
            BB->SetValueAsBool(TEXT("IsSearching"), true);
            MonsterController->ClearFocus(EAIFocusPriority::Gameplay);
            UE_LOG(LogTemp, Warning, TEXT("[SERVICE] Path Fail  → LastKnownLocation SET,  Now=%.2f"), Now);
        }
    }

    if (!bPathFail && bPrevPathFail)
    {
        BB->SetValueAsBool(TEXT("IsSearching"), false);
    }

    if (bPlayerInRange)
    {
        BB->ClearValue(TEXT("PatrolTarget"));
        return;
    }

    const bool bSearching =
        (BB->IsVectorValueSet(TEXT("LastKnownLocation")) &&
            BB->GetValueAsFloat(TEXT("SearchUntilTime")) > 0.f &&
            Now < BB->GetValueAsFloat(TEXT("SearchUntilTime")));

    if (bSearching)
    {
        BB->ClearValue(TEXT("PatrolTarget"));

        const FVector LK = BB->GetValueAsVector(TEXT("LastKnownLocation"));

        if (BB->IsVectorValueSet(TEXT("SearchPoint")))
        {
            const FVector Cur = BB->GetValueAsVector(TEXT("SearchPoint"));
            const float DistSq = FVector::DistSquared(Monster->GetActorLocation(), Cur);
            if (DistSq < FMath::Square(10.f))
                BB->ClearValue(TEXT("SearchPoint"));
        }

        if (LK != FVector::ZeroVector && !BB->IsVectorValueSet(TEXT("SearchPoint")))
        {
            if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(World))
            {
                FNavLocation Out;
                if (Nav->GetRandomReachablePointInRadius(LK, 1500.f, Out))
                    BB->SetValueAsVector(TEXT("SearchPoint"), Out.Location);
                else
                    BB->SetValueAsVector(TEXT("SearchPoint"), LK);
            }
        }

        return;
    }

    const int32 NumTargets = Monster->PatrolTargets.Num();
    if (NumTargets > 0)
    {
        const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
        if (AActor* Target = Monster->PatrolTargets[RandomIndex])
            BB->SetValueAsObject(TEXT("PatrolTarget"), Target);
    }
}