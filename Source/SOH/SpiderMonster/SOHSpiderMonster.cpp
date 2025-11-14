#include "SOHSpiderMonster.h"
#include "SOHSpiderMonsterAIController.h"
#include "SOH/AI/SOHAIMonster.h"
#include "SOH/AI/SOHAIMonsterController.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ASOHSpiderMonster::ASOHSpiderMonster()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASOHSpiderMonsterAIController::StaticClass();
}

void ASOHSpiderMonster::BeginPlay()
{
	Super::BeginPlay();

	if (!MainMonsterRef)
	{
		for (TActorIterator<ASOHAIMonster> It(GetWorld()); It; ++It)
		{
			MainMonsterRef = *It;
			break;
		}
	}
}

void ASOHSpiderMonster::OnPlayerSpotted(AActor* PlayerActor)
{
	if (!PlayerActor)
	{
		return;
	}

	if (ScreamSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ScreamSound,
			GetActorLocation()
		);
	}

	if (!MainMonsterRef)
	{
		return;
	}

	if (AAIController* AIC = Cast<AAIController>(MainMonsterRef->GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			const FVector AlertLocation = GetActorLocation();

			BB->SetValueAsVector(
				ASOHAIMonsterController::Key_SpiderAlertLocation,
				AlertLocation
			);
		}
	}
}