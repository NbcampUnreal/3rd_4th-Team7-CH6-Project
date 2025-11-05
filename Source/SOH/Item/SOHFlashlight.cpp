#include "SOHFlashlight.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

ASOHFlashlight::ASOHFlashlight()
{
    PrimaryActorTick.bCanEverTick = true;

    // 루트와 기준 생성
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
    Pivot->SetupAttachment(Root);

    // 외형 생성
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Pivot);

    // 라이트 생성
    Spot = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot"));
    Spot->SetupAttachment(Pivot);
    Spot->bUseInverseSquaredFalloff = false;
    Spot->SetIntensity(4000.f);

    HandSocketName = TEXT("FlashlightSocket"); // 손 소켓 이름
    IntensityOn = 4000.f; // 밝기
    bOn = false; // 꺼진 상태로 시작
    bEquipped = false; // 장착전

    //배치시 상태 라이트 꺼짐
    Spot->SetVisibility(false, true);
    Spot->SetHiddenInGame(true);
    Spot->SetIntensity(0.f);
}

void ASOHFlashlight::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 장착시 카메라 방향을 업데이트해서 회전
    if (bEquipped) UpdateToCamera();
}

void ASOHFlashlight::UpdateToCamera()
{
    if (OwnerCam && Pivot)
        Pivot->SetWorldRotation(OwnerCam->GetComponentRotation()); // 카메라 회전값 복사
}

void ASOHFlashlight::SetOn(bool bEnable)
{
    // 라이트 on/off 설정
    bOn = bEnable;
    Spot->SetVisibility(bOn, true);
    Spot->SetHiddenInGame(!bOn);
    Spot->SetIntensity(bOn ? IntensityOn : 0.f);
}

void ASOHFlashlight::Toggle()
{
    // F키 입력 시 호출 — 켜져있으면 끄고, 꺼져있으면 켬
    SetOn(!bOn);
}

// 줍기
void ASOHFlashlight::InteractPickup(ACharacter* Picker)
{
    if (!Picker) return;

    // Owner 정보
    OwnerChar = Picker;
    OwnerCam = Picker->FindComponentByClass<UCameraComponent>();
    OwnerMesh = Picker->GetMesh();

    //방어코드? 소켓이름이랑 맞는 소켓이 없으면 return 합니다.
    if (!OwnerMesh || !OwnerMesh->DoesSocketExist(HandSocketName)) return;

    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttachToComponent(OwnerMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        HandSocketName); // 붙일 소켓 이름

    bEquipped = true; // 손에 들린 상태 SetOn함수에서 쓰임 들려있어야만 킬 수 있게끔.
    SetOn(false);
}