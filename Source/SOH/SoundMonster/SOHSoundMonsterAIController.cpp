#include "SOHSoundMonsterAIController.h"
#include "SOHSoundMonster.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

const FName ASOHSoundMonsterAIController::Key_PlayerActor(TEXT("PlayerActor"));
const FName ASOHSoundMonsterAIController::Key_PlayerInRange(TEXT("PlayerInRange"));
const FName ASOHSoundMonsterAIController::Key_PatrolTarget(TEXT("PatrolTarget"));

ASOHSoundMonsterAIController::ASOHSoundMonsterAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(
		TEXT("/Game/AI/SoundMonster/SoundMonsterController/BT_SoundMonster.BT_SoundMonster")
	);
	if (BTAsset.Succeeded())
	{
		BehaviorTreeAsset = BTAsset.Object;
	}

	if (SightConfig)
	{
		SightConfig->SightRadius = 1500.f;
		SightConfig->LoseSightRadius = 1800.f;
		SightConfig->PeripheralVisionAngleDegrees = 70.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	}

	if (HearingConfig)
	{
		HearingConfig->HearingRange = 1500.f;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	}

	if (PerceptionComp)
	{
		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->ConfigureSense(*HearingConfig);
		PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &ASOHSoundMonsterAIController::HandleTargetPerceptionUpdated
		);
	}
}

void ASOHSoundMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeAsset || !BlackboardComp)
		return;

	UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp);

	BlackboardComp->SetValueAsBool(Key_PlayerInRange, false);
	BlackboardComp->ClearValue(Key_PlayerActor);
	BlackboardComp->ClearValue(Key_PatrolTarget);

	RunBehaviorTree(BehaviorTreeAsset);
}

void ASOHSoundMonsterAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !BlackboardComp) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Actor != PlayerPawn) return;

	const bool bSensed = Stimulus.WasSuccessfullySensed();

	if (bSensed)
	{

		BlackboardComp->SetValueAsObject(Key_PlayerActor, PlayerPawn);
		BlackboardComp->SetValueAsBool(Key_PlayerInRange, true);

		static const FAISenseID HearingSenseID =
			UAISense::GetSenseID(UAISense_Hearing::StaticClass());

		if (Stimulus.Type == HearingSenseID)
		{
			BlackboardComp->SetValueAsVector(
				TEXT("HeardSoundLocation"),
				Stimulus.StimulusLocation
			);
		}
	}
	else
	{

		BlackboardComp->SetValueAsBool(Key_PlayerInRange, false);
		BlackboardComp->ClearValue(Key_PlayerActor);

		ClearFocus(EAIFocusPriority::Gameplay);
	}
}
