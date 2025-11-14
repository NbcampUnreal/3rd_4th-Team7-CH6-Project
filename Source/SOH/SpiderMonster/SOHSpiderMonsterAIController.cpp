#include "SOHSpiderMonsterAIController.h"
#include "SOHSpiderMonster.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

ASOHSpiderMonsterAIController::ASOHSpiderMonsterAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	SightConfig->SightRadius = 1300.f;
	SightConfig->LoseSightRadius = 1500.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
		this,
		&ASOHSpiderMonsterAIController::HandleTargetPerceptionUpdated
	);

	PatrolRadius = 2000.0f;
	PatrolInterval = 6.0f;

	bIsStopped = false;
}

void ASOHSpiderMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (PerceptionComp && SightConfig)
	{
		PerceptionComp->RequestStimuliListenerUpdate();
	}

	StartPatrol();
}

void ASOHSpiderMonsterAIController::StartPatrol()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PatrolTimerHandle,
			this,
			&ASOHSpiderMonsterAIController::MoveToRandomPatrolLocation,
			PatrolInterval,
			true,
			0.f
		);
	}
}

void ASOHSpiderMonsterAIController::MoveToRandomPatrolLocation()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation OutLocation;
		if (NavSys->GetRandomReachablePointInRadius(
			ControlledPawn->GetActorLocation(),
			PatrolRadius,
			OutLocation))
		{
			MoveToLocation(OutLocation.Location);
		}
	}
}

void ASOHSpiderMonsterAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Actor != PlayerPawn)
	{
		return;
	}

	ASOHSpiderMonster* Spider = Cast<ASOHSpiderMonster>(GetPawn());
	if (!Spider)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		bIsStopped = true;
		StopMovement();

		FVector ToPlayer = PlayerPawn->GetActorLocation() - Spider->GetActorLocation();
		ToPlayer.Z = 0;
		FRotator NewRot = ToPlayer.Rotation();
		Spider->SetActorRotation(NewRot);

		GetWorld()->GetTimerManager().SetTimer(
			StopTimerHandle,
			[this]()
			{
				bIsStopped = false;
			},
			5.0f,
			false
		);

		Spider->OnPlayerSpotted(PlayerPawn);
	}
}
