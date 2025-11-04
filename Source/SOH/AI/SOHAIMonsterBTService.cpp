#include "SOHAIMonsterBTService.h"
#include "SOHAIMonsterController.h"
#include "SOHAIMonster.h"
#include "Engine/TargetPoint.h"
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

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	ASOHAIMonster* Monster = Cast<ASOHAIMonster>(MonsterController->GetPawn());
	if (!Monster) return;

	const bool bPlayerInRange = BB->GetValueAsBool(TEXT("PlayerInRange"));
	if (bPlayerInRange)
	{
		BB->ClearValue(TEXT("PatrolTarget"));
		return;
	}

	const int32 NumTargets = Monster->PatrolTargets.Num();
	if (NumTargets > 0)
	{
		//AActor* Current = Cast<AActor>(BB->GetValueAsObject(TEXT("PatrolTarget")));
		//if (!Current)
		//{
			const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
			AActor* Target = Monster->PatrolTargets[RandomIndex];
			if (Target)
			{
				BB->SetValueAsObject(TEXT("PatrolTarget"), Target);
			}
		//}
	}
}
