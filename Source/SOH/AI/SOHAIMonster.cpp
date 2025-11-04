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

	auto* Move = GetCharacterMovement();
	if (Move)
	{
		Move->bOrientRotationToMovement = true;
		Move->bUseControllerDesiredRotation = false;
		Move->RotationRate = FRotator(0.f, 420.f, 0.f);
	}

	bUseControllerRotationYaw = false;

	PatrolSpeed = 300.f;
	ChaseSpeed = 500.f;

	SightRadius = 2000.f;
	LoseSightRadius = 2400.f;
	PeripheralVisionAngle = 80.f;

	HearingRange = 1500.f;
}

void ASOHAIMonster::BeginPlay()
{
	Super::BeginPlay();
	SetMoveSpeed(PatrolSpeed);
}


void ASOHAIMonster::SetMoveSpeed(float NewSpeed)
{
	if (auto* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = NewSpeed;
	}
}