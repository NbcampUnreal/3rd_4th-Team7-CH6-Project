#include "SOHAIMonster.h"
#include "SOH/Interface/SOHDoorInterface.h" 
#include "Engine/TargetPoint.h"
#include "SOHAIMonsterController.h"
#include "AIController.h"
#include "SOHPatrolRoute.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"

ASOHAIMonster::ASOHAIMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASOHAIMonsterController::StaticClass();

    ChaseAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ChaseAudioComp"));
    ChaseAudioComp->SetupAttachment(RootComponent);
    ChaseAudioComp->bAutoActivate = false;

    ArriveAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ArriveAudioComp"));
    ArriveAudioComp->SetupAttachment(RootComponent);
    ArriveAudioComp->bAutoActivate = false;

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

    if (!PatrolRouteActor)
    {
        TArray<AActor*> FoundRoutes;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASOHPatrolRoute::StaticClass(), FoundRoutes);

        if (FoundRoutes.Num() > 0)
        {
            PatrolRouteActor = Cast<ASOHPatrolRoute>(FoundRoutes[0]);

            float MinDist = FLT_MAX;
            for (AActor* Actor : FoundRoutes)
            {
                float Dist = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
                if (Dist < MinDist)
                {
                    MinDist = Dist;
                    PatrolRouteActor = Cast<ASOHPatrolRoute>(Actor);
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
    if (!bSoundEnabled) return;

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
}

void ASOHAIMonster::PlayArriveAtTargetSound()
{
    if (!bSoundEnabled) return;
    if (!ArriveAtTargetSound) return;
    if (!ArriveAudioComp) return;

    ArriveAudioComp->SetSound(ArriveAtTargetSound);
    ArriveAudioComp->Play();
}

void ASOHAIMonster::PlayHearNoiseSound()
{
    if (!bSoundEnabled) return;

    if (!HearNoiseSound) return;

    UWorld* World = GetWorld();
    if (!World) return;

    UGameplayStatics::PlaySoundAtLocation(
        World,
        HearNoiseSound,
        GetActorLocation()
    );
}

void ASOHAIMonster::StartInvestigateNoise(const FVector& NoiseLocation, AAIController* InController)
{
    if (bInvestigatingNoise || !Controller)
        return;

    bInvestigatingNoise = true;
    CachedAIController = Cast<AAIController>(InController);

    UCharacterMovementComponent* Move = GetCharacterMovement();
    if (Move)
    {
        Move->StopMovementImmediately();
        Move->DisableMovement();
    }

    FVector Dir = NoiseLocation - GetActorLocation();
    Dir.Z = 0;
    if (!Dir.IsNearlyZero())
    {
        FRotator NewRot = Dir.Rotation();
        SetActorRotation(NewRot);
    }

    PlayHearNoiseSound();

    if (SeePlayerDuringInvestigate())
    {
        if (CachedAIController)
        {
            CachedAIController->GetBlackboardComponent()->SetValueAsBool(
                TEXT("PlayerInRange"), true
            );
        }

        if (UCharacterMovementComponent* MoveMonster = GetCharacterMovement())
        {
            MoveMonster->SetMovementMode(MOVE_Walking);
        }

        //EndInvestigateNoise();
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(
        NoiseInvestigateTimerHandle,
        this,
        &ASOHAIMonster::EndInvestigateNoise,
        NoiseInvestigateDuration,
        false
    );
}

void ASOHAIMonster::EndInvestigateNoise()
{
    bInvestigatingNoise = false;

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->SetMovementMode(MOVE_Walking);
    }

    if (CachedAIController)
    {
        CachedAIController->ClearFocus(EAIFocusPriority::Gameplay);
        CachedAIController = nullptr;
    }
}

bool ASOHAIMonster::SeePlayerDuringInvestigate()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return false;

    return HasLineOfSightToTarget(Player);
}

void ASOHAIMonster::StopAllMontagesInstant()
{
    if (USkeletalMeshComponent* MontageMesh = GetMesh())
    {
        if (UAnimInstance* Anim = MontageMesh->GetAnimInstance())
        {
            Anim->StopAllMontages(0.0f);
        }
    }
}

void ASOHAIMonster::PlayChaseLoop()
{
    if (!bSoundEnabled) return;
    if (!ChaseSound) return;
    if (!ChaseAudioComp) return;

    if (!ChaseAudioComp->IsPlaying())
    {
        ChaseAudioComp->SetSound(ChaseSound);
        ChaseAudioComp->Play();
    }
}

void ASOHAIMonster::StopChaseLoop()
{
    if (!ChaseAudioComp) return;

    if (ChaseAudioComp->IsPlaying())
    {
        ChaseAudioComp->Stop();
    }
}

void ASOHAIMonster::StopArriveSound()
{
    if (!ArriveAudioComp) return;

    if (ArriveAudioComp->IsPlaying())
    {
        ArriveAudioComp->Stop();
    }
}