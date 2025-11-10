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


//void USOHAIMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
//{
//	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
//
//
//
//	AAIController* MonsterController = OwnerComp.GetAIOwner();
//	if (!MonsterController) return;
//
//	UWorld* World = MonsterController->GetWorld();
//	if (!World) return;
//
//	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
//	if (!BB) return;
//
//	ASOHAIMonster* Monster = Cast<ASOHAIMonster>(MonsterController->GetPawn());
//	if (!Monster) return;
//
//	const bool bPlayerInRange = BB->GetValueAsBool(TEXT("PlayerInRange"));
//
//	AActor* PlayerActor = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
//	bool bInAttackRange = false;
//
//	if (PlayerActor)
//	{
//		const float DistSq = FVector::DistSquared(
//			PlayerActor->GetActorLocation(),
//			Monster->GetActorLocation()
//		);
//
//		const float Range = Monster->AttackRange;
//		bInAttackRange = DistSq <= FMath::Square(Range);
//	}
//
//	BB->SetValueAsBool(TEXT("AttackRange"), bInAttackRange);
//
//	if (bPlayerInRange && PlayerActor)
//	{
//		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
//		if (NavSys)
//		{
//			const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
//			if (NavData)
//			{
//				FPathFindingQuery Query(MonsterController, *NavData, Monster->GetActorLocation(), PlayerActor->GetActorLocation());
//				const FPathFindingResult Result = NavSys->FindPathSync(Query);
//
//				if (!Result.IsSuccessful() || !Result.Path.IsValid() || Result.IsPartial())
//				{
//					const float Now = World->GetTimeSeconds();
//
//					const bool bHasLK = BB->IsVectorValueSet(TEXT("LastKnownLocation"));
//					const float UntilExisting = BB->GetValueAsFloat(TEXT("SearchUntilTime"));
//					const bool bTimerActive = (UntilExisting > Now);
//
//					if (!(bHasLK && bTimerActive))
//					{
//						BB->SetValueAsVector(TEXT("LastKnownLocation"), PlayerActor->GetActorLocation());
//						BB->SetValueAsFloat(TEXT("SearchUntilTime"), Now + 10.f);
//						MonsterController->ClearFocus(EAIFocusPriority::Gameplay);
//					}
//				}
//			}
//		}
//	}
//
//	if (bPlayerInRange)
//	{
//		BB->ClearValue(TEXT("PatrolTarget"));
//		return;
//	}
//
//	const float Now = World->GetTimeSeconds();
//	const float Until = BB->GetValueAsFloat(TEXT("SearchUntilTime"));
//
//	if (Until > 0.f && Now >= Until)
//	{
//		BB->ClearValue(TEXT("LastKnownLocation"));
//		BB->ClearValue(TEXT("SearchPoint"));
//		BB->ClearValue(TEXT("SearchUntilTime"));
//	}
//
//	const bool bSearching =
//		(BB->IsVectorValueSet(TEXT("LastKnownLocation")) &&
//			BB->GetValueAsFloat(TEXT("SearchUntilTime")) > 0.f &&
//			Now < BB->GetValueAsFloat(TEXT("SearchUntilTime")));
//
//	if (bSearching)
//	{
//		BB->ClearValue(TEXT("PatrolTarget"));
//
//		const FVector LK = BB->GetValueAsVector(TEXT("LastKnownLocation"));
//		FVector SearchPoint = BB->GetValueAsVector(TEXT("SearchPoint"));
//
//		if (BB->IsVectorValueSet(TEXT("SearchPoint")))
//		{
//			const float DistSq = FVector::DistSquared(Monster->GetActorLocation(), SearchPoint);
//			if (DistSq < FMath::Square(10.f))
//			{
//				BB->ClearValue(TEXT("SearchPoint"));
//			}
//		}
//
//		if (LK != FVector::ZeroVector && !BB->IsVectorValueSet(TEXT("SearchPoint")))
//		{
//			if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
//			{
//				FNavLocation Out;
//				if (Nav->GetRandomReachablePointInRadius(LK, 1500.f, Out))
//					BB->SetValueAsVector(TEXT("SearchPoint"), Out.Location);
//				else
//					BB->SetValueAsVector(TEXT("SearchPoint"), LK);
//			}
//		}
//
//		return;
//	}
//
//	const int32 NumTargets = Monster->PatrolTargets.Num();
//	if (NumTargets > 0)
//	{
//			const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
//			AActor* Target = Monster->PatrolTargets[RandomIndex];
//			if (Target)
//			{
//				BB->SetValueAsObject(TEXT("PatrolTarget"), Target);
//			}
//	}
//}

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

    // --- AttackRange 계산 (유지) ---
    bool bInAttackRange = false;
    if (PlayerActor)
    {
        const float DistSq = FVector::DistSquared(PlayerActor->GetActorLocation(), Monster->GetActorLocation());
        bInAttackRange = (DistSq <= FMath::Square(Monster->AttackRange));
    }
    BB->SetValueAsBool(TEXT("AttackRange"), bInAttackRange);                                     // :contentReference[oaicite:0]{index=0}

    // ===== 만료 우선 정리 =====
    const float Now = World->GetTimeSeconds();
    const float Until = BB->GetValueAsFloat(TEXT("SearchUntilTime"));
    if (Until > 0.f && Now >= Until)                                                              // CHANGED
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVICE] Expired Search → LastKnownLocation CLEAR, Now=%.2f, Until=%.2f"), Now, Until); // 디버그
        BB->ClearValue(TEXT("LastKnownLocation"));                                                // CHANGED
        BB->ClearValue(TEXT("SearchPoint"));                                                      // CHANGED
        BB->ClearValue(TEXT("SearchUntilTime"));                                                  // CHANGED
        BB->SetValueAsBool(TEXT("IsSearching"), false);                                           // CHANGED
        MonsterController->ClearFocus(EAIFocusPriority::Gameplay);                                 // CHANGED
    }

    // ===== 경로 상태 평가 =====
    bool bPathFail = false;                                                                       // CHANGED: 현재 틱 경로 실패 여부
    if (bPlayerInRange && PlayerActor)
    {
        if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
        {
            if (const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate))
            {
                FPathFindingQuery Query(MonsterController, *NavData, Monster->GetActorLocation(), PlayerActor->GetActorLocation());
                const FPathFindingResult Result = NavSys->FindPathSync(Query);
                bPathFail = (!Result.IsSuccessful() || !Result.Path.IsValid() || Result.IsPartial()); // CHANGED
            }
        }
    }

    // ===== 에지 검출 플래그 =====
    const bool bPrevPathFail = BB->GetValueAsBool(TEXT("PathFailing"));                           // CHANGED: BB bool 새로 추가
    if (bPathFail != bPrevPathFail)                                                               // CHANGED: 에지 업데이트
        BB->SetValueAsBool(TEXT("PathFailing"), bPathFail);                                       // CHANGED

    // ===== '실패로 전환'되는 그 순간에만 탐색 진입 =====
    if (bPlayerInRange && PlayerActor && bPathFail && !bPrevPathFail)                             // CHANGED: 진입 에지
    {
        //const float Now = World->GetTimeSeconds();                                                // CHANGED
        // 이미 탐색 중이면 재무장 금지
        const bool  bHasLK = BB->IsVectorValueSet(TEXT("LastKnownLocation"));                     // CHANGED
        const float UntilExisting = BB->GetValueAsFloat(TEXT("SearchUntilTime"));                 // CHANGED
        const bool  bTimerActive = (UntilExisting > Now);                                         // CHANGED

        if (!(bHasLK && bTimerActive))                                                            // CHANGED: 첫 진입일 때만
        {
            const FVector NewLK = PlayerActor->GetActorLocation();                                // CHANGED
            BB->SetValueAsVector(TEXT("LastKnownLocation"), NewLK);                               // CHANGED
            BB->SetValueAsFloat(TEXT("SearchUntilTime"), Now + 10.f);                             // CHANGED
            BB->SetValueAsBool(TEXT("IsSearching"), true);                                        // CHANGED: (선택) 보기 쉬운 플래그
            MonsterController->ClearFocus(EAIFocusPriority::Gameplay);                            // CHANGED
            UE_LOG(LogTemp, Warning, TEXT("[SERVICE] Path Fail  → LastKnownLocation SET,  Now=%.2f"), Now); // 디버그
        }
    }

    // 경로가 회복되면(성공 상태로 돌아오면) 탐색·타이머 재무장 허용을 위해 플래그 리셋
    if (!bPathFail && bPrevPathFail)                                                              // CHANGED: 회복 에지
    {
        BB->SetValueAsBool(TEXT("IsSearching"), false);                                           // CHANGED
        // (여기서 타이머를 건드리진 않음)
    }

    // --- PlayerInRange면 순찰 타겟은 비우고 종료 ---
    if (bPlayerInRange)
    {
        BB->ClearValue(TEXT("PatrolTarget"));
        return;
    }

    // ===== 탐색 상태 계산 (Now < Until 포함) =====
    const bool bSearching =
        (BB->IsVectorValueSet(TEXT("LastKnownLocation")) &&
            BB->GetValueAsFloat(TEXT("SearchUntilTime")) > 0.f &&
            Now < BB->GetValueAsFloat(TEXT("SearchUntilTime")));                                     // CHANGED

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

    // --- 순찰 타겟 랜덤 선택 (유지) ---
    const int32 NumTargets = Monster->PatrolTargets.Num();
    if (NumTargets > 0)
    {
        const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
        if (AActor* Target = Monster->PatrolTargets[RandomIndex])
            BB->SetValueAsObject(TEXT("PatrolTarget"), Target);
    }
}