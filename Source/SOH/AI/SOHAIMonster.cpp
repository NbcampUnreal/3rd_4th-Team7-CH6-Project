#include "SOHAIMonster.h"
#include "Engine/TargetPoint.h"
#include "SOHAIMonsterController.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
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

	SightRadius = 1000.f;
	LoseSightRadius = 1300.f;
	PeripheralVisionAngle = 80.f;

	HearingRange = 1500.f;

	AttackDamage = 20.0f;
	AttackRange = 200.0f;
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

void ASOHAIMonster::TryAttack()
{
	AActor* Target = nullptr;
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			Target = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
		}
	}
	if (!Target || Target == this) return;

    if (AttackMontage)
    {
        if (UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
        {
            Anim->Montage_Play(AttackMontage);
        }
    }

    UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, nullptr);
}