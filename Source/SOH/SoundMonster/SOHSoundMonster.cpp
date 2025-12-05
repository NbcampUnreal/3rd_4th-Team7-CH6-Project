#include "SOHSoundMonster.h"
#include "SOHSoundMonsterPatrolRoute.h"
#include "SOHSoundMonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Interaction/SOHInteractInterface.h"

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

	if (!PatrolRouteActor)
	{
		TArray<AActor*> FoundRoutes;
		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			ASOHSoundMonsterPatrolRoute::StaticClass(),
			FoundRoutes
		);

		if (FoundRoutes.Num() > 0)
		{
			float MinDist = FLT_MAX;
			for (AActor* Actor : FoundRoutes)
			{
				float Dist = FVector::DistSquared(GetActorLocation(), Actor->GetActorLocation());
				if (Dist < MinDist)
				{
					MinDist = Dist;
					PatrolRouteActor = Cast<ASOHSoundMonsterPatrolRoute>(Actor);
				}
			}
		}
	}

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

void ASOHSoundMonster::CheckDoorAhead()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector End = Start + Forward * 150.f;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SoundMonsterDoorCheck), false);
	Params.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_GameTraceChannel1,
		Params
	);

	if (!bHit) return;

	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return;

	if (HitActor->GetClass()->ImplementsInterface(USOHInteractInterface::StaticClass()))
	{
		ISOHInteractInterface::Execute_Interact(HitActor, this);
	}
}