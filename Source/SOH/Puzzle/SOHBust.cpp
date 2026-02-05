#include "Puzzle/SOHBust.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ASOHBust::ASOHBust()
{
    PrimaryActorTick.bCanEverTick = true;

    // ��Ʈ �޽�
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    // Ÿ�Ӷ���
    RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
    RotationTimeline->SetNetAddressable();
}

void ASOHBust::BeginPlay()
{
    Super::BeginPlay();

    if (!RotationCurve || !RotationTimeline) return;

    StartRotation = StaticMesh->GetRelativeRotation();

    // Ÿ�Ӷ��� ������Ʈ ���ε�
    FOnTimelineFloat UpdateFunc;
    UpdateFunc.BindUFunction(this, FName("HandleTimelineUpdate"));
    RotationTimeline->AddInterpFloat(RotationCurve, UpdateFunc);

    // Ÿ�Ӷ��� ���� ���ε�
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

    // ? �̹� ȸ�� ���̸� ���� ? ���� ���� ����
    if (RotationTimeline->IsPlaying())
        return;

    if (RotateSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, RotateSound, GetActorLocation());
    }

    // ���� �˻�� Position ���� (0~3)
    Position = (Position + 1) % 4;

    OnBustRotated.Broadcast();

    // ���� ȸ���� �������� ����
    StartRotation = StaticMesh->GetRelativeRotation();

    // ��ǥ ȸ�� 90�� ����
    TargetRotation = StartRotation + FRotator(0.f, YawPerPosition, 0.f);

    // ȸ�� ����
    RotationTimeline->PlayFromStart();
}

void ASOHBust::HandleTimelineUpdate(float Alpha)
{
    // Start �� Target �� �ε巴�� ����
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
    // �ʿ� ���� (���� ȸ������ StartRotation�� ���ŵ�)
}

void ASOHBust::SaveState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone())
        return;

    FWorldStateData& Data =
        SaveData->WorldStateMap.FindOrAdd(WorldStateID);

    Data.IntValue = Position;
}

void ASOHBust::LoadState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone())
        return;

    if (FWorldStateData* Data =
        SaveData->WorldStateMap.Find(WorldStateID))
    {
        Position = Data->IntValue;

        // ⭐ 회전 복원
        const float Yaw = Position * YawPerPosition;
        StaticMesh->SetRelativeRotation(FRotator(0.f, Yaw, 0.f));
    }
}
