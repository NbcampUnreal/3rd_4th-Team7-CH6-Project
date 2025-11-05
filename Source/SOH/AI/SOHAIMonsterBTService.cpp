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
		const float DistSq = FVector::DistSquared(
			PlayerActor->GetActorLocation(),
			Monster->GetActorLocation()
		);

		const float Range = Monster->AttackRange;
		bInAttackRange = DistSq <= FMath::Square(Range);
	}

	BB->SetValueAsBool(TEXT("AttackRange"), bInAttackRange);

	if (bPlayerInRange && PlayerActor)
	{
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
		if (NavSys)
		{
			const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
			if (NavData)
			{
				FPathFindingQuery Query(MonsterController, *NavData, Monster->GetActorLocation(), PlayerActor->GetActorLocation());
				const FPathFindingResult Result = NavSys->FindPathSync(Query);

				if (!Result.IsSuccessful() || !Result.Path.IsValid() || Result.IsPartial())
				{
					BB->SetValueAsBool(TEXT("PlayerInRange"), false);
					BB->ClearValue(TEXT("PlayerActor"));

					MonsterController->ClearFocus(EAIFocusPriority::Gameplay);

					if (ASOHAIMonsterController* MC = Cast<ASOHAIMonsterController>(MonsterController))
					{
						MC->RestoreDetectAll();
					}
				}
			}
		}
	}

	if (bPlayerInRange)
	{
		BB->ClearValue(TEXT("PatrolTarget"));
		return;
	}

	const int32 NumTargets = Monster->PatrolTargets.Num();
	if (NumTargets > 0)
	{
			const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
			AActor* Target = Monster->PatrolTargets[RandomIndex];
			if (Target)
			{
				BB->SetValueAsObject(TEXT("PatrolTarget"), Target);
			}
	}
}
