#include "Monster/SOHAIMonsterController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ASOHAIMonsterController::ASOHAIMonsterController()
{

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/Monster/MonsterController/BT_AIMonster.BT_AIMonster"));
	if (BTAsset.Succeeded())
	{
		BehaviorTreeAsset = BTAsset.Object;
	}
}

void ASOHAIMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

