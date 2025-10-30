#include "SOHAIMonster.h"
#include "Engine/TargetPoint.h"
#include "SOHAIMonsterController.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ASOHAIMonster::ASOHAIMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASOHAIMonsterController::StaticClass();

	PatrolSpeed = 400.0f;
}

void ASOHAIMonster::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

