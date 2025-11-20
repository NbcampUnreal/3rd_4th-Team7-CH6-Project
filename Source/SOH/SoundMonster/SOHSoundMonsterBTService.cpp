#include "SOHSoundMonsterBTService.h"
#include "SOHSoundMonsterAIController.h"
#include "SOHSoundMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

USOHSoundMonsterBTService::USOHSoundMonsterBTService()
{
	NodeName = TEXT("SoundMonster Patrol Or Chase");
	Interval = 0.3f;
	RandomDeviation = 0.05f;
}

void USOHSoundMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ASOHSoundMonsterAIController* MonsterController = Cast<ASOHSoundMonsterAIController>(OwnerComp.GetAIOwner());
	if (!MonsterController)
		return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
		return;

	APawn* ControlledPawn = MonsterController->GetPawn();
	ASOHSoundMonster* Monster = ControlledPawn ? Cast<ASOHSoundMonster>(ControlledPawn) : nullptr;
	if (!Monster)
		return;

	const bool bPlayerInRange = BB->GetValueAsBool(ASOHSoundMonsterAIController::Key_PlayerInRange);
	UObject* PlayerObj = BB->GetValueAsObject(ASOHSoundMonsterAIController::Key_PlayerActor);
	AActor* PlayerActor = Cast<AActor>(PlayerObj);

	if (bPlayerInRange && PlayerActor)
	{
		MonsterController->SetFocus(PlayerActor);

		BB->ClearValue(ASOHSoundMonsterAIController::Key_PatrolTarget);
		return;
	}

	MonsterController->ClearFocus(EAIFocusPriority::Gameplay);

	const int32 NumTargets = Monster->PatrolTargets.Num();
	if (NumTargets <= 0)
		return;

	const int32 RandomIndex = FMath::RandRange(0, NumTargets - 1);
	if (AActor* Target = Monster->PatrolTargets[RandomIndex])
	{
		BB->SetValueAsObject(ASOHSoundMonsterAIController::Key_PatrolTarget, Target);
	}
}
