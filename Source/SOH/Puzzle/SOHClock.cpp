#include "Puzzle/SOHClock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ASOHClock::ASOHClock()
{
    PrimaryActorTick.bCanEverTick = true;

    MinuteHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MinuteHand"));
    RootComponent = MinuteHand;
}

void ASOHClock::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 마우스 입력 바인딩 (우클릭 또는 좌클릭 원하는 버튼 선택)
        PC->InputComponent->BindAction("LeftMouse", IE_Pressed, this, &ASOHClock::OnMouseDown);
        PC->InputComponent->BindAction("LeftMouse", IE_Released, this, &ASOHClock::OnMouseUp);
    }
}

void ASOHClock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsDragging) return;

    float NewAngle = GetMouseAngle();
    float RotAmount = (NewAngle - StartDragAngle);

    // 라디안 → 도
    RotAmount = FMath::RadiansToDegrees(RotAmount);

    CurrentAngle += RotAmount;

    // 분침 회전 적용
    FRotator R(0, 0, CurrentAngle);
    MinuteHand->SetRelativeRotation(R);

    // 다음 프레임 기준점 업데이트
    StartDragAngle = NewAngle;
}

void ASOHClock::OnMouseDown()
{
    // 화면 → 월드 변환 실패하면 무시
    float MouseAngle = GetMouseAngle();
    if (MouseAngle == 99999.0f) return;

    bIsDragging = true;
    StartDragAngle = MouseAngle;
}

void ASOHClock::OnMouseUp()
{
    bIsDragging = false;
}

float ASOHClock::GetMouseAngle()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return 99999.0f;

    float MouseX, MouseY;
    PC->GetMousePosition(MouseX, MouseY);

    FVector WorldLoc, WorldDir;
    if (!PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLoc, WorldDir))
        return 99999.0f;

    // 카메라 위치
    FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();

    // 충분히 멀리 쏨
    FVector TargetPos = CamLoc + WorldDir * 2000.f;

    // 회전 중심(분침 Pivot 위치)
    FVector Pivot = MinuteHand->GetComponentLocation();

    FVector Delta = TargetPos - Pivot;

    // atan2(Y, X)
    return FMath::Atan2(Delta.Y, Delta.X);
}
