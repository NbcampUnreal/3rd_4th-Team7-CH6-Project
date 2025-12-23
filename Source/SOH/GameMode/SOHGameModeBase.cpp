#include "SOHGameModeBase.h"
#include "SOHGameInstance.h"
#include "Character/SOHPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void ASOHGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FString MapName = GetWorld()->GetMapName();
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay: %s"), *MapName);
    UE_LOG(LogTemp, Log, TEXT("===================================="));
}

void ASOHGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    if (!NewPlayer) return;

    APawn* PlayerPawn = NewPlayer->GetPawn();
    if (!PlayerPawn) return;

    ASOHPlayerCharacter* Character = Cast<ASOHPlayerCharacter>(PlayerPawn);
    if (!Character) return;

    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (!GI) return;

    if (GI->bLoadedFromSave)
    {
        UE_LOG(LogTemp, Log, TEXT("[GameMode] ✅ Continue: Stage %d"), GI->CurrentStage);
        Character->ApplyLoadedData();
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[GameMode] 🆕 New Game: PlayerStart"));
    }
}

// ⭐ 새 게임
void ASOHGameModeBase::StartGame()
{
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    UE_LOG(LogTemp, Log, TEXT("🆕 StartGame: New Game"));
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    
    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (!GI) return;

    // 새 게임 데이터 초기화
    GI->ResetGameData();
    
    FString CurrentLevel = GetWorld()->GetMapName();
    
    if (CurrentLevel.Contains("MainMenu") || CurrentLevel.Contains(LobbyLevelName.ToString()))
    {
        UGameplayStatics::OpenLevel(this, GameLevelName);
    }
}

// ⭐ 이어하기
void ASOHGameModeBase::ContinueGame()
{
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    UE_LOG(LogTemp, Log, TEXT("▶️ ContinueGame: Load Game"));
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    
    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (!GI) return;

    // 세이브 데이터 로드
    if (!GI->LoadGameData())
    {
        UE_LOG(LogTemp, Error, TEXT("[GameMode] ❌ Failed to load"));
        return;
    }
    
    FString CurrentLevel = GetWorld()->GetMapName();
    
    if (CurrentLevel.Contains("MainMenu") || CurrentLevel.Contains(LobbyLevelName.ToString()))
    {
        UGameplayStatics::OpenLevel(this, GameLevelName);
    }
}

void ASOHGameModeBase::OnGameClear()
{
    UE_LOG(LogTemp, Warning, TEXT("🎉 Game Clear!"));
    
    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (GI)
    {
        GI->SaveGameData();
    }
    
    ShowGameClearUI();
}

void ASOHGameModeBase::OnPlayerDied()
{
    UE_LOG(LogTemp, Error, TEXT("💀 Player Died"));
    
    ShowGameOverUI();

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;
    if (!DeathWidgetClass) return;

    if (!DeathWidgetInstance)
    {
        DeathWidgetInstance = CreateWidget<UUserWidget>(PC, DeathWidgetClass);
    }

    if (DeathWidgetInstance && !DeathWidgetInstance->IsInViewport())
    {
        DeathWidgetInstance->AddToViewport();

        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(DeathWidgetInstance->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}