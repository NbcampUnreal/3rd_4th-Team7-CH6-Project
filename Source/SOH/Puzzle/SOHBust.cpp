#include "Puzzle/SOHBust.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ASOHBust::ASOHBust()
{
    PrimaryActorTick.bCanEverTick = true;

    // 루트 메쉬
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    // 타임라인
    RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
    RotationTimeline->SetNetAddressable();
}

void ASOHBust::BeginPlay()
{
    Super::BeginPlay();

    if (!RotationCurve || !RotationTimeline) return;

    StartRotation = StaticMesh->GetRelativeRotation();

    // 타임라인 업데이트 바인딩
    FOnTimelineFloat UpdateFunc;
    UpdateFunc.BindUFunction(this, FName("HandleTimelineUpdate"));
    RotationTimeline->AddInterpFloat(RotationCurve, UpdateFunc);

    // 타임라인 종료 바인딩
    FOnTimelineEvent FinishFunc;
    FinishFunc.BindUFunction(this, FName("HandleTimelineFinished"));
    RotationTimeline->SetTimelineFinishedFunc(FinishFunc);
}

void ASOHBust::Interact_Implementation(AActor* Caller)
{
    if (bIsLocked)
        return;
    
    if (!RotationTimeline || !RotationCurve)
        return;

    // ? 이미 회전 중이면 무시 ? 오차 완전 제거
    if (RotationTimeline->IsPlaying())
        return;

    if (RotateSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, RotateSound, GetActorLocation());
    }

    // 퍼즐 검사용 Position 증가 (0~3)
    Position = (Position + 1) % 4;

    OnBustRotated.Broadcast();

    // 현재 회전을 시작으로 설정
    StartRotation = StaticMesh->GetRelativeRotation();

    // 목표 회전 90도 증가
    TargetRotation = StartRotation + FRotator(0.f, YawPerPosition, 0.f);

    // 회전 시작
    RotationTimeline->PlayFromStart();
}

void ASOHBust::HandleTimelineUpdate(float Alpha)
{
    // Start → Target 을 부드럽게 보간
    FRotator NewRot = UKismetMathLibrary::RLerp(
        StartRotation,
        TargetRotation,
        Alpha,
        true   // Shortest Path
    );

    StaticMesh->SetRelativeRotation(NewRot);
}

void ASOHBust::HandleTimelineFinished()
{
    // 필요 없음 (다음 회전에서 StartRotation이 갱신됨)
}
