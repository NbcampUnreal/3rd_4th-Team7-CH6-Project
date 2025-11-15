#include "SOHAIMonsterController.h"
#include "SOHAIMonster.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

const FName ASOHAIMonsterController::Key_PlayerActor(TEXT("PlayerActor"));
const FName ASOHAIMonsterController::Key_PlayerInRange(TEXT("PlayerInRange"));
const FName ASOHAIMonsterController::Key_PatrolTarget(TEXT("PatrolTarget"));

const FName ASOHAIMonsterController::Key_LastKnownLocation(TEXT("LastKnownLocation"));
const FName ASOHAIMonsterController::Key_SearchPoint(TEXT("SearchPoint"));
const FName ASOHAIMonsterController::Key_SearchUntilTime(TEXT("SearchUntilTime"));
const FName ASOHAIMonsterController::Key_PathFailing(TEXT("PathFailing"));
const FName ASOHAIMonsterController::Key_IsSearching(TEXT("IsSearching"));
const FName ASOHAIMonsterController::Key_PlayerOnNav(TEXT("PlayerOnNav"));

const FName ASOHAIMonsterController::Key_SpiderAlertLocation(TEXT("SpiderAlertLocation"));

ASOHAIMonsterController::ASOHAIMonsterController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/AI/MonsterController/BT_Monster.BT_Monster"));
	if (BTAsset.Succeeded())
	{
		BehaviorTreeAsset = BTAsset.Object;
	}

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;
	PerceptionComp->RequestStimuliListenerUpdate();

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->ConfigureSense(*HearingConfig);
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASOHAIMonsterController::HandleTargetPerceptionUpdated);
}

void ASOHAIMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp);
		RunBehaviorTree(BehaviorTreeAsset);
	}

	if (const ASOHAIMonster* Monster = Cast<ASOHAIMonster>(InPawn))
	{
		SightConfig->SightRadius = Monster->SightRadius;
		SightConfig->LoseSightRadius = Monster->LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = Monster->PeripheralVisionAngle;

		HearingConfig->HearingRange = Monster->HearingRange;

		PerceptionComp->RequestStimuliListenerUpdate();
	}
}

void ASOHAIMonsterController::SetDetectOnlyPlayer()
{
	if (!SightConfig || !PerceptionComp) return;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	PerceptionComp->RequestStimuliListenerUpdate();
}

void ASOHAIMonsterController::RestoreDetectAll()
{
	if (!SightConfig || !PerceptionComp) return;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	PerceptionComp->RequestStimuliListenerUpdate();
}

void ASOHAIMonsterController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !BlackboardComp) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Actor != PlayerPawn) return;

	const bool bSensed = Stimulus.WasSuccessfullySensed();

	if (bSensed)
	{
		BlackboardComp->SetValueAsObject(Key_PlayerActor, PlayerPawn);
		BlackboardComp->SetValueAsBool(Key_PlayerInRange, true);
		BlackboardComp->ClearValue(Key_LastKnownLocation);

		BlackboardComp->ClearValue(Key_SearchPoint);
		BlackboardComp->SetValueAsFloat(Key_SearchUntilTime, 0.f);
		BlackboardComp->SetValueAsBool(Key_IsSearching, false);

		if (SightConfig && PerceptionComp)
		{
			SightConfig->AutoSuccessRangeFromLastSeenLocation = SightConfig->SightRadius;
			PerceptionComp->RequestStimuliListenerUpdate();
		}

		SetFocus(PlayerPawn);
	}
	else
	{
		BlackboardComp->SetValueAsBool(Key_PlayerInRange, false);
		BlackboardComp->ClearValue(Key_PlayerActor);

		BlackboardComp->SetValueAsBool(Key_PlayerOnNav, false);

		if (SightConfig && PerceptionComp)
		{
			SightConfig->AutoSuccessRangeFromLastSeenLocation = 0.f;
			PerceptionComp->RequestStimuliListenerUpdate();
		}

		ClearFocus(EAIFocusPriority::Gameplay);

		GetPerceptionComponent()->ForgetAll();
	}
}

void ASOHAIMonsterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!BlackboardComp) return;

	if (Result.IsSuccess())
	{
		if (ASOHAIMonster* Monster = Cast<ASOHAIMonster>(GetPawn()))
		{
			Monster->PlayLookAroundMontage();
		}
	}

	if (Result.IsSuccess() && BlackboardComp->IsVectorValueSet(Key_SpiderAlertLocation))
	{
		BlackboardComp->ClearValue(Key_SpiderAlertLocation);
	}

	if (BlackboardComp->IsVectorValueSet(Key_SearchPoint))
	{
		BlackboardComp->ClearValue(Key_SearchPoint);
	}
}