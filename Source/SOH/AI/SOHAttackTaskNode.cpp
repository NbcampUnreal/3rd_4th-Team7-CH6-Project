#include "SOHAttackTaskNode.h"
#include "SOHAIMonster.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

USOHAttackTaskNode::USOHAttackTaskNode()
{
	NodeName = TEXT("Monster Attack");
	bNotifyTick = false;
}

EBTNodeResult::Type USOHAttackTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
		return EBTNodeResult::Failed;

	ASOHAIMonster* Monster = Cast<ASOHAIMonster>(AIC->GetPawn());
	if (!Monster)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
		return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Target || Target == Monster)
		return EBTNodeResult::Succeeded;

	const float DistSq = FVector::DistSquared(Monster->GetActorLocation(), Target->GetActorLocation());
	if (DistSq > FMath::Square(Monster->AttackRange))
		return EBTNodeResult::Succeeded;

	if (!Monster->HasLineOfSightToTarget(Target))
	{
		return EBTNodeResult::Succeeded;
	}

	Monster->TryAttack();

	return EBTNodeResult::Succeeded;
}
