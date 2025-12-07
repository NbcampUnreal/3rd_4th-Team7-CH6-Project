#include "SOHJumpScareBase.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameMode/SOHGameInstance.h"

ASOHJumpScareBase::ASOHJumpScareBase()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldStatic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

    JumpScareCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("JumpScareCamera"));
    JumpScareCamera->SetupAttachment(RootComponent);

    ScareSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ScareSpawnPoint"));
    ScareSpawnPoint->SetupAttachment(RootComponent);
}

void ASOHJumpScareBase::BeginPlay()
{
    Super::BeginPlay();

    if (TriggerBox && bUseOverlapTrigger)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(
            this,
            &ASOHJumpScareBase::OnTriggerBeginOverlap
        );
    }

    // 플레이어/컨트롤러 저장
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            CachedPlayerController = PC;
            if (APawn* Pawn = PC->GetPawn())
            {
                CachedPlayerCharacter = Cast<ACharacter>(Pawn);
            }
        }
    }

    if (UWorld* World = GetWorld())
    {
        if (USOHGameInstance* GI = World->GetGameInstance<USOHGameInstance>())
        {
            GI->OnConditionCompleted.AddDynamic(
                this,
                &ASOHJumpScareBase::OnConditionCompleted
            );

            // 초기 상태 체크
            if (bUseActivationTag && GI->HasCondition(ActivationConditionTag))
            {
                bCanActivateByTag = true;
            }
            if (bUseDeactivationTag && GI->HasCondition(DeactivationConditionTag))
            {
                bCanActivateByTag = false;
                bAlreadyTriggered = true;
            }
        }
    }
}

void ASOHJumpScareBase::OnConditionCompleted(FGameplayTag CompletedTag)
{
    if (bUseActivationTag && CompletedTag.MatchesTagExact(ActivationConditionTag))
    {
        bCanActivateByTag = true;
    }

    if (bUseDeactivationTag && CompletedTag.MatchesTagExact(DeactivationConditionTag))
    {
        bCanActivateByTag = false;
        bAlreadyTriggered = true;

        if (bIsJumpScarePlaying)
        {
            FinishJumpScare();
        }
    }
}

void ASOHJumpScareBase::OnTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!bUseOverlapTrigger) return;

    if (bAlreadyTriggered && bOneShot) return;

    if (!OtherActor) return;

    if (bUseActivationTag && !bCanActivateByTag) return;

    if (CachedPlayerCharacter.IsValid() && OtherActor == CachedPlayerCharacter.Get())
    {
        StartJumpScare(OtherActor);
    }
}

// BP나 다른 C++에서 호출해 점프스퀘어 실행가능
void ASOHJumpScareBase::StartJumpScare(AActor* TriggeringActor)
{
    if (bUseActivationTag && !bCanActivateByTag) return;

    if (bAlreadyTriggered && bOneShot) return;

    if (bIsJumpScarePlaying) return;

    InternalStartJumpScare(TriggeringActor);
}

// 점프스퀘어 시작
void ASOHJumpScareBase::InternalStartJumpScare(AActor* TriggeringActor)
{
    UWorld* World = GetWorld();

    if (!World) return;

    if (bIsJumpScarePlaying) return;

    // 플레이어/컨트롤러 캐시
    if (!CachedPlayerController.IsValid())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            CachedPlayerController = PC;
        }
    }

    if (!CachedPlayerCharacter.IsValid() && CachedPlayerController.IsValid())
    {
        if (APawn* Pawn = CachedPlayerController->GetPawn())
        {
            CachedPlayerCharacter = Cast<ACharacter>(Pawn);
        }
    }

    if (!CachedPlayerController.IsValid()) return;

    bIsJumpScarePlaying = true;
    bAlreadyTriggered = true;

    APlayerController* PC = CachedPlayerController.Get();

    // 입력 잠금
    if (bLockPlayerInput && PC)
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    // Pause
    if (bPauseGameDuringJumpScare)
    {
        UGameplayStatics::SetGamePaused(World, true);
    }

    // 카메라 전환
    if (bUseJumpScareCamera && PC)
    {
        PC->SetViewTargetWithBlend(this, CameraBlendTime);
    }

    // 연출용 액터 스폰
    if (ScareActorClass)
    {
        const FTransform SpawnTransform = ScareSpawnPoint
            ? ScareSpawnPoint->GetComponentTransform()
            : GetActorTransform();

        FActorSpawnParameters Params;
        Params.Owner = this;

        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (AActor* NewActor = World->SpawnActor<AActor>(ScareActorClass, SpawnTransform, Params))
        {
            SpawnedScareActor = NewActor;
        }
    }

    // 몽타주 재생 (옵션)
    if (JumpScareMontage && SpawnedScareActor.IsValid())
    {
        if (ACharacter* MonsterChar = Cast<ACharacter>(SpawnedScareActor.Get()))
        {
            if (USkeletalMeshComponent* MeshComp = MonsterChar->GetMesh())
            {
                if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
                {
                    AnimInst->Montage_Play(JumpScareMontage, 1.0f);
                }
            }
        }
    }

    if (JumpScareSound)
    {
        // 스폰 위치 기준 재생
        FVector SoundLoc = SpawnedScareActor.IsValid()
            ? SpawnedScareActor->GetActorLocation()
            : GetActorLocation();

        UGameplayStatics::PlaySoundAtLocation(this, JumpScareSound, SoundLoc);
    }

    // 연출 시간 타이머
    if (JumpScareDuration > 0.0f)
    {
        World->GetTimerManager().SetTimer(
            JumpscareTimerHandle,
            this,
            &ASOHJumpScareBase::InternalEndJumpScare,
            JumpScareDuration,
            false
        );
    }

    OnJumpScareStarted(TriggeringActor);
}

// BP나 C++에서 종료가능
void ASOHJumpScareBase::FinishJumpScare()
{
    if (!bIsJumpScarePlaying) return;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(JumpscareTimerHandle);
    }

    InternalEndJumpScare();
}

// 점프스퀘어 종료
void ASOHJumpScareBase::InternalEndJumpScare()
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (!CachedPlayerController.IsValid())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            CachedPlayerController = PC;
        }
    }

    if (!CachedPlayerCharacter.IsValid() && CachedPlayerController.IsValid())
    {
        if (APawn* Pawn = CachedPlayerController->GetPawn())
        {
            CachedPlayerCharacter = Cast<ACharacter>(Pawn);
        }
    }

    APlayerController* PC = CachedPlayerController.Get();

    // Pause 해제
    if (bPauseGameDuringJumpScare)
    {
        UGameplayStatics::SetGamePaused(World, false);
    }

    // 입력 복구
    if (bLockPlayerInput && PC)
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
    }

    // 카메라 복구
    if (bUseJumpScareCamera && PC && CachedPlayerCharacter.IsValid())
    {
        PC->SetViewTargetWithBlend(CachedPlayerCharacter.Get(), CameraBlendTime);
    }

    // 스폰된 액터 제거
    if (SpawnedScareActor.IsValid())
    {
        SpawnedScareActor->Destroy();
        SpawnedScareActor = nullptr;
    }

    bIsJumpScarePlaying = false;

    // BP 연결
    OnJumpScareEnded();

    // 이 점프스퀘어 Destroy
    if (bOneShot)
    {
        Destroy();
    }
}