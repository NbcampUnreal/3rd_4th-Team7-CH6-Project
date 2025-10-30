#include "SOHAIMonsterBTService.h"
#include "SOHAIMonsterController.h"
#include "SOHAIMonster.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"

USOHAIMonsterBTService::USOHAIMonsterBTService()
{
	NodeName = TEXT("Set Random Patrol Target");
	Interval = 0.2f;
	RandomDeviation = 0.0f;
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

	const int32 NumTargets = Monster->PatrolTargets.Num();
	if (NumTargets <= 0) return;

	const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
	AActor* Target = Monster->PatrolTargets[RandomIndex];
	if (!Target) return;

	BB->SetValueAsObject(TEXT("PatrolTarget"), Target);
}
