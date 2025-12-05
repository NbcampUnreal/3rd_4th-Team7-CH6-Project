#include "SOHAIMonster.h"
#include "SOH/Interface/SOHDoorInterface.h" 
#include "Engine/TargetPoint.h"
#include "SOHAIMonsterController.h"
#include "AIController.h"
#include "SOHPatrolRoute.h"
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

	PatrolSpeed = 200.f;
	ChaseSpeed = 600.f;

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

    if (PatrolTargets.Num() == 0 && PatrolRouteActor)
    {
        PatrolTargets = PatrolRouteActor->PatrolPoints;
    }

	SetMoveSpeed(PatrolSpeed);
}


void ASOHAIMonster::SetMoveSpeed(float NewSpeed)
{
	if (auto* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = NewSpeed;
	}
}

void ASOHAIMonster::PlayLookAroundMontage()
{
    if (!LookAroundMontage) return;

    if (UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
    {
        Anim->Montage_Play(LookAroundMontage);
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

bool ASOHAIMonster::HasLineOfSightToTarget(AActor* Target)
{
    if (!Target)
        return false;

    UWorld* World = GetWorld();
    if (!World)
        return false;

    FVector Start = GetActorLocation();
    FVector End = Target->GetActorLocation();

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MonsterLOS), false);
    Params.AddIgnoredActor(this);

    const bool bHit = World->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_GameTraceChannel1,
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

    if (HitActor->GetClass()->ImplementsInterface(USOHDoorInterface::StaticClass()))
    {
        ISOHDoorInterface::Execute_OpenDoorForAI(HitActor, this);
        return false;
    }

    return false;
}

void ASOHAIMonster::CheckDoorAhead()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Start = GetActorLocation();
    FVector Forward = GetActorForwardVector();
    FVector End = Start + Forward * 150.f; // 앞쪽 150 거리 확인

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MonsterDoorCheck), false);
    Params.AddIgnoredActor(this);

    bool bHit = World->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_GameTraceChannel1,
        Params
    );

    if (!bHit) return;

    AActor* HitActor = Hit.GetActor();
    if (!HitActor) return;

    if (HitActor->GetClass()->ImplementsInterface(USOHDoorInterface::StaticClass()))
    {
        ISOHDoorInterface::Execute_OpenDoorForAI(HitActor, this);
    }
}

void ASOHAIMonster::PlayDetectPlayerSound()
{
    if (!DetectPlayerSound)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    UGameplayStatics::PlaySoundAtLocation(
        World,
        DetectPlayerSound,
        GetActorLocation()
    );

    if (ChaseSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            World,
            ChaseSound,
            GetActorLocation()
        );
    }
}