#include "Puzzle/SOHCombinationLock.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

ASOHCombinationLock::ASOHCombinationLock()
{
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
    Gear1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear1"));
    Gear2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear2"));
    Gear3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear3"));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    
    Body->SetupAttachment(RootComponent);
    Head->SetupAttachment(RootComponent);
    Gear1->SetupAttachment(RootComponent);
    Gear2->SetupAttachment(RootComponent);
    Gear3->SetupAttachment(RootComponent);
    CameraComponent->SetupAttachment(RootComponent);

    Gears.Add(Gear1);
    Gears.Add(Gear2);
    Gears.Add(Gear3);
}

void ASOHCombinationLock::Interact_Implementation(AActor* Caller)
{
    Super::Interact_Implementation(Caller);

    ACharacter* PlayerChar = Cast<ACharacter>(Caller);
    if (!PlayerChar) return;
    APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
    if (!PC) return;
    
    PlayerChar->GetCharacterMovement()->DisableMovement();
    PC->SetIgnoreLookInput(true);
    EnableInput(PC);
    
    PC->SetViewTargetWithBlend(this, 0.5f);\
    
    if (InputComponent)
    {
        InputComponent->KeyBindings.Empty();
        InputComponent->BindKey(EKeys::W, IE_Pressed, this, &ASOHCombinationLock::Up);
        InputComponent->BindKey(EKeys::S, IE_Pressed, this, &ASOHCombinationLock::Down);
        InputComponent->BindKey(EKeys::A, IE_Pressed, this, &ASOHCombinationLock::Left);
        InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ASOHCombinationLock::Right);
        InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ASOHCombinationLock::Exit);

    }
    
    CurrentGearIndex = 0;
    GearValues.Init(0, Gears.Num());
    //오버레이 초기화
    Body->SetOverlayMaterial(nullptr);
    Head->SetOverlayMaterial(nullptr);
    for (auto* Gear : Gears)
    {
        Gear->SetOverlayMaterial(nullptr);
    }
    //회전값 초기화
    for (int32 i = 0; i < Gears.Num(); i++)
    {
        if (Gears[i])
        {
            Gears[i]->SetRelativeRotation(FRotator::ZeroRotator);
        }
    }
    for (int32 i = 0; i < GearValues.Num(); i++)
    {
        GearValues[i] = 0;
    }

    if (Gears.IsValidIndex(CurrentGearIndex) && OutlineMaterial)
    {
        Gears[CurrentGearIndex]->SetOverlayMaterial(OutlineMaterial);
    }
    
    if (LockGuideWidgetClass && !LockGuideWidget)
    {
        LockGuideWidget = CreateWidget<UUserWidget>(PC, LockGuideWidgetClass);
        if (LockGuideWidget)
        {
            LockGuideWidget->AddToViewport();
        }
    }
}
void ASOHCombinationLock::Up()
{
    const int32 GearCount = Gears.Num();
    if (GearCount == 0) return;

    int32 OldIndex = CurrentGearIndex;
    CurrentGearIndex = (CurrentGearIndex - 1 + Gears.Num()) % Gears.Num();
    // 하이라이트 갱신
    if (Gears.IsValidIndex(OldIndex))
        Gears[OldIndex]->SetOverlayMaterial(nullptr);

    if (Gears.IsValidIndex(CurrentGearIndex))
        Gears[CurrentGearIndex]->SetOverlayMaterial(OutlineMaterial);
}
void ASOHCombinationLock::Down()
{
    const int32 GearCount = Gears.Num();
    if (GearCount == 0) return;

    int32 OldIndex = CurrentGearIndex;
    CurrentGearIndex = (CurrentGearIndex + 1) % GearCount;

    // 하이라이트 갱신
    if (Gears.IsValidIndex(OldIndex))
        Gears[OldIndex]->SetOverlayMaterial(nullptr);

    if (Gears.IsValidIndex(CurrentGearIndex))
        Gears[CurrentGearIndex]->SetOverlayMaterial(OutlineMaterial);
}
void ASOHCombinationLock::Left()
{
    
    UStaticMeshComponent* SelectedGear = Gears[CurrentGearIndex];

    GearValues[CurrentGearIndex] = (GearValues[CurrentGearIndex] - 1 + 10) % 10;
    FRotator Rot = SelectedGear->GetRelativeRotation();
    Rot.Yaw = GearValues[CurrentGearIndex] * 36.f; 
    SelectedGear->SetRelativeRotation(Rot);

    UE_LOG(LogTemp, Warning, TEXT("Gear %d Rotated to %d"), CurrentGearIndex, GearValues[CurrentGearIndex]);
    CheckCombination();
}
void ASOHCombinationLock::Right()
{
    UStaticMeshComponent* SelectedGear = Gears[CurrentGearIndex];
    GearValues[CurrentGearIndex] = (GearValues[CurrentGearIndex] + 1) % 10;
    FRotator Rot = SelectedGear->GetRelativeRotation();
    Rot.Yaw = GearValues[CurrentGearIndex] * 36.f;
    SelectedGear->SetRelativeRotation(Rot);

    UE_LOG(LogTemp, Warning, TEXT("Gear %d Rotated to %d"), CurrentGearIndex, GearValues[CurrentGearIndex]);
    CheckCombination();
}
void ASOHCombinationLock::Exit()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;
    ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
    if (!PlayerChar) return;

    DisableInput(PC);
    PC->SetIgnoreLookInput(false);
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PlayerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    PC->SetViewTargetWithBlend(PlayerChar, 0.5f);
    
    if (LockGuideWidget)
    {
        LockGuideWidget->RemoveFromParent();
        LockGuideWidget = nullptr;
    }
}
void ASOHCombinationLock::CheckCombination()
{
    if (GearValues.Num() != CorrectCombination.Num())
        return;

    for (int32 i = 0; i < GearValues.Num(); ++i)
    {
        if (GearValues[i] != CorrectCombination[i].CurrentDigit)
            return;
    }
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;
    ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
    if (!PlayerChar) return;
    DisableInput(PC);
    PC->SetIgnoreLookInput(false);
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PlayerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    PC->SetViewTargetWithBlend(PlayerChar, 0.5f);
    if (LockGuideWidget)
    {
        LockGuideWidget->RemoveFromParent();
        LockGuideWidget = nullptr;
    }
    Destroy();
}