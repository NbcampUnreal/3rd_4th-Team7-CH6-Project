#include "Puzzle/SOHBust.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"

ASOHBust::ASOHBust()
{
    PrimaryActorTick.bCanEverTick = true;

    // 루트에 붙는 메쉬 컴포넌트 생성
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    // 타임라인 컴포넌트 생성
    RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
    RotationTimeline->SetNetAddressable();
}

void ASOHBust::BeginPlay()
{
    Super::BeginPlay();

    if (!StaticMesh || !RotationTimeline || !RotationCurve) return;

    // 초기 회전값 저장
    StartRotation = StaticMesh->GetRelativeRotation();

    // 타임라인 업데이트 바인딩
    FOnTimelineFloat UpdateFunction;
    UpdateFunction.BindUFunction(this, FName("HandleTimelineUpdate"));
    RotationTimeline->AddInterpFloat(RotationCurve, UpdateFunction, FName("RotationTrack"));

    // 타임라인 종료 바인딩
    FOnTimelineEventStatic FinishedFunction;
    FinishedFunction.BindUFunction(this, FName("HandleTimelineFinished"));
    RotationTimeline->SetTimelineFinishedFunc(FinishedFunction);

    // 초기 Position 설정
    Position = 0;
    YawPerPosition = 90.f;
}

void ASOHBust::HandleTimelineUpdate(float Alpha)
{
    if (!StaticMesh) return;

    // 목표 Yaw = 누적 Position 기반
    float TargetYaw = Position * YawPerPosition;

    // StartRotation 기반 Lerp
    float CurrentYaw = UKismetMathLibrary::Lerp(StartRotation.Yaw, TargetYaw, Alpha);

    // 0~360 범위로 보정
    CurrentYaw = FMath::Fmod(CurrentYaw + 360.f, 360.f);

    // 새로운 회전값 생성 및 적용
    FRotator NewRotation = StartRotation;
    NewRotation.Yaw = CurrentYaw;
    StaticMesh->SetRelativeRotation(NewRotation);

    UE_LOG(LogTemp, Warning, TEXT("Rotating: Yaw %.2f / Position %d"), CurrentYaw, Position);
}

void ASOHBust::HandleTimelineFinished()
{
    // 다음 회전도 자연스럽게 누적되도록 현재 회전을 StartRotation으로 갱신
    if (StaticMesh)
    {
        StartRotation = StaticMesh->GetRelativeRotation();
    }
}

void ASOHBust::Interact_Implementation(AActor* Caller)
{
    if (!StaticMesh || !RotationTimeline) return;

    // Position 누적 (0~3 반복)
    Position = (Position + 1) % 4;

    // 현재 회전값을 StartRotation으로 갱신
    StartRotation = StaticMesh->GetRelativeRotation();

    UE_LOG(LogTemp, Warning, TEXT("StartRotation - Pitch: %.2f, Yaw: %.2f, Roll: %.2f"),
        StartRotation.Pitch, StartRotation.Yaw, StartRotation.Roll);

    // 타임라인 재생
    RotationTimeline->PlayFromStart();

    UE_LOG(LogTemp, Warning, TEXT("Rotate! Current Position: %d"), Position);
}
