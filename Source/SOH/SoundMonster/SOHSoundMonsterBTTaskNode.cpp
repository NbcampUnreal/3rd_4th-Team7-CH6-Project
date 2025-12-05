#include "SOHSoundMonsterBTTaskNode.h"
#include "SOHSoundMonsterAIController.h"
#include "SOHSoundMonster.h"
#include "BehaviorTree/BlackboardComponent.h"

USOHSoundMonsterBTTaskNode::USOHSoundMonsterBTTaskNode()
{
    NodeName = TEXT("Attack Player (SoundMonster)");
}

EBTNodeResult::Type USOHSoundMonsterBTTaskNode::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASOHSoundMonsterAIController* AIC =
        Cast<ASOHSoundMonsterAIController>(OwnerComp.GetAIOwner());
    if (!AIC) return EBTNodeResult::Failed;

    ASOHSoundMonster* Monster = Cast<ASOHSoundMonster>(AIC->GetPawn());
    if (!Monster) return EBTNodeResult::Failed;

    Monster->TryAttack();

    return EBTNodeResult::Succeeded;
}
