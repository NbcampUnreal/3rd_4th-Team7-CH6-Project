#include "SOHSoundMonster.h"
#include "SOHSoundMonsterPatrolRoute.h"
#include "SOHSoundMonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "SOH/Interface/SOHDoorInterface.h"

ASOHSoundMonster::ASOHSoundMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASOHSoundMonsterAIController::StaticClass();

	if (auto* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = true;
		Move->bUseControllerDesiredRotation = false;
		Move->RotationRate = FRotator(0.f, 420.f, 0.f);
	}

	bUseControllerRotationYaw = false;

	PatrolSpeed = 200.f;
	ChaseSpeed = 400.f;

	SightRadius = 1000.f;
	LoseSightRadius = 1300.f;
	PeripheralVisionAngle = 80.f;

	HearingRange = 1500.f;
}

void ASOHSoundMonster::BeginPlay()
{
	Super::BeginPlay();

	if (PatrolTargets.Num() == 0 && PatrolRouteActor)
	{
		PatrolTargets = PatrolRouteActor->PatrolPoints;
	}

	SetMoveSpeed(PatrolSpeed);
}

void ASOHSoundMonster::SetMoveSpeed(float NewSpeed)
{
	if (auto* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = NewSpeed;
	}
}

bool ASOHSoundMonster::HasLineOfSightToTarget(AActor* Target)
{
	if (!Target)
		return false;

	UWorld* World = GetWorld();
	if (!World)
		return false;

	FVector Start = GetActorLocation();
	FVector End = Target->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SoundMonsterLOS), false);
	Params.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		return true;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
		return false;

	if (HitActor == Target)
	{
		return true;
	}

	return false;
}

void ASOHSoundMonster::TryAttack()
{
	AActor* Target = nullptr;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			Target = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
		}
	}

	if (!Target || Target == this)
		return;

	float DistSq = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());
	if (DistSq > FMath::Square(AttackRange))
		return;

	UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, nullptr);
}