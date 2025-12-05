#include "Puzzle/SOHCombinationLock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Level/SOHOpenDoor.h"

// Constructor
ASOHCombinationLock::ASOHCombinationLock()
{
    PrimaryActorTick.bCanEverTick = true;

    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
    Gear1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear1"));
    Gear2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear2"));
    Gear3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear3"));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

    // Attach components
    RootComponent = Body;
    Head->SetupAttachment(RootComponent);
    Gear1->SetupAttachment(RootComponent);
    Gear2->SetupAttachment(RootComponent);
    Gear3->SetupAttachment(RootComponent);
    CameraComponent->SetupAttachment(RootComponent);

    Gears = { Gear1, Gear2, Gear3 };

    // Initialize arrays
    GearCurrentRot.Init(FRotator::ZeroRotator, Gears.Num());
    GearTargetRot.Init(FRotator::ZeroRotator, Gears.Num());
    GearValues.Init(0, Gears.Num());
}

// BeginPlay
void ASOHCombinationLock::BeginPlay()
{
    Super::BeginPlay();

    if (Head)
        HeadStartLocation = Head->GetRelativeLocation();

    HeadTargetLocation = HeadStartLocation;
    // UpdateHighlight();
}

// Tick
void ASOHCombinationLock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Smooth gear rotation
    for (int32 i = 0; i < Gears.Num(); ++i)
    {
        if (!Gears[i]) continue;
        FRotator NewRot = FMath::RInterpConstantTo(Gears[i]->GetRelativeRotation(), GearTargetRot[i], DeltaTime, GearLerpSpeed);
        Gears[i]->SetRelativeRotation(NewRot);
    }

    // Head movement
    if (bIsHeadMoving && Head)
    {
        FVector NewLoc = FMath::VInterpConstantTo(Head->GetRelativeLocation(), HeadTargetLocation, DeltaTime, HeadMoveSpeed);
        Head->SetRelativeLocation(NewLoc);

        if (NewLoc.Equals(HeadTargetLocation, 0.1f))
        {
            bIsHeadMoving = false;
            UnlockComplete();
        }
    }
}

// Interact
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

    PC->SetViewTargetWithBlend(this, 0.5f);

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
    GearCurrentRot.Init(FRotator::ZeroRotator, Gears.Num());
    GearTargetRot.Init(FRotator::ZeroRotator, Gears.Num());

    Body->SetOverlayMaterial(nullptr);
    Head->SetOverlayMaterial(nullptr);
    for (auto* Gear : Gears)
    {
        if (Gear)
            Gear->SetOverlayMaterial(nullptr);
    }
    // UpdateHighlight();

    if (LockGuideWidgetClass && !LockGuideWidget)
    {
        LockGuideWidget = CreateWidget<UUserWidget>(PC, LockGuideWidgetClass);
        if (LockGuideWidget)
            LockGuideWidget->AddToViewport();
    }
}

// Input functions
void ASOHCombinationLock::Up()
{
    CurrentGearIndex = (CurrentGearIndex - 1 + Gears.Num()) % Gears.Num();
    // UpdateHighlight();
}

void ASOHCombinationLock::Down()
{
    CurrentGearIndex = (CurrentGearIndex + 1) % Gears.Num();
    // UpdateHighlight();
}

void ASOHCombinationLock::Left()
{
    if (!Gears.IsValidIndex(CurrentGearIndex)) return;
    GearTargetRot[CurrentGearIndex] += FRotator(0.f, -36.f, 0.f);
    GearValues[CurrentGearIndex] = (GearValues[CurrentGearIndex] - 1 + 10) % 10;

    if (GearRotateSound)
        UGameplayStatics::PlaySoundAtLocation(this, GearRotateSound, GetActorLocation());

    CheckCombination();
}

void ASOHCombinationLock::Right()
{
    if (!Gears.IsValidIndex(CurrentGearIndex)) return;
    GearTargetRot[CurrentGearIndex] += FRotator(0.f, 36.f, 0.f);
    GearValues[CurrentGearIndex] = (GearValues[CurrentGearIndex] + 1) % 10;

    if (GearRotateSound)
        UGameplayStatics::PlaySoundAtLocation(this, GearRotateSound, GetActorLocation());

    CheckCombination();
}

void ASOHCombinationLock::Exit()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;
    ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
    
    DisableInput(PC); 
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->SetIgnoreLookInput(false);
    if (PlayerChar)
    {
        PlayerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
    PC->SetViewTargetWithBlend(PlayerChar, 0.5f);
    
    if (LockGuideWidget)
    {
        LockGuideWidget->RemoveFromParent();
        LockGuideWidget = nullptr;
    }
}

// Update gear highlight
// void ASOHCombinationLock::UpdateHighlight()
// {
//     for (int32 i = 0; i < Gears.Num(); ++i)
//     {
//         if (Gears[i])
//             Gears[i]->SetOverlayMaterial(i == CurrentGearIndex ? OutlineMaterial : nullptr);
//     }
// }

// Check combination
void ASOHCombinationLock::CheckCombination()
{
    if (GearValues.Num() != CorrectCombination.Num()) return;

    for (int32 i = 0; i < GearValues.Num(); ++i)
        if (GearValues[i] != CorrectCombination[i].CurrentDigit)
            return;

    PlayUnlockAnimation();
}

// Play unlock animation
void ASOHCombinationLock::PlayUnlockAnimation()
{
    if (!Head) return;
    HeadTargetLocation = HeadStartLocation + FVector(0, 0, 1.f);
    bIsHeadMoving = true;

    if (UnlockSound)
        UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, Head->GetComponentLocation());
}

// Unlock complete
void ASOHCombinationLock::UnlockComplete()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
    
    PC->SetViewTargetWithBlend(PlayerChar, 0.5f);
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->SetIgnoreLookInput(false);
    if (PlayerChar)
    {
        PlayerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
    
    if (LockGuideWidget)
    {
        LockGuideWidget->RemoveFromParent();
        LockGuideWidget = nullptr;
    }

    if (LinkedDoor)
    {
        LinkedDoor->UnlockOpenDoor(this);
    }

    FTimerHandle DestroyTimerHandle;
    FTimerDelegate DestroyDelegate;
    DestroyDelegate.BindLambda([this]()
    {
        if (IsValid(this))
        {
            Destroy();
        }
    });
    GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle,DestroyDelegate,0.5,false );
}
