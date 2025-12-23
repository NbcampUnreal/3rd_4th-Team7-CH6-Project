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
    
    if (CurrentLevel.Contains("MainMenuLevel") || CurrentLevel.Contains(LobbyLevelName.ToString()))
    {
        UGameplayStatics::OpenLevel(this, GameLevelName);
    }
}

// ⭐ 이어하기
// SOHGameModeBase.cpp
void ASOHGameModeBase::ContinueGame()
{
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    UE_LOG(LogTemp, Log, TEXT("▶️ ContinueGame"));
    UE_LOG(LogTemp, Log, TEXT("===================================="));
    
    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ GameInstance null"));
        return;
    }

    // 세이브 파일 로드
    if (!GI->LoadGameData())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 로드 실패"));
        return;
    }
    
    FString CurrentLevel = GetWorld()->GetMapName();
    UE_LOG(LogTemp, Log, TEXT("📍 현재 레벨: %s"), *CurrentLevel);
    
    // 메인 메뉴에서 호출
    if (CurrentLevel.Contains("MainMenu") || CurrentLevel.Contains(LobbyLevelName.ToString()))
    {
        UE_LOG(LogTemp, Log, TEXT("🎬 메인 메뉴 → 게임 레벨로 이동"));
        UGameplayStatics::OpenLevel(this, GameLevelName);
    }
    else
    {
        // 게임 중 호출
        UE_LOG(LogTemp, Warning, TEXT("🎮 게임 중 로드 - 데이터 적용"));
        
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (!PC)
        {
            UE_LOG(LogTemp, Error, TEXT("❌ PlayerController null"));
            return;
        }

        ASOHPlayerCharacter* PlayerCharacter = Cast<ASOHPlayerCharacter>(PC->GetPawn());
        if (!PlayerCharacter)
        {
            UE_LOG(LogTemp, Error, TEXT("❌ PlayerCharacter null"));
            return;
        }

        // 데이터 적용
        PlayerCharacter->ApplyLoadedData();
        
        // ⭐ 0.1초 후 한 번 더 Input Mode 설정 (확실하게!)
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PC]()
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
            UE_LOG(LogTemp, Warning, TEXT("⏰ 0.1초 후 Input Mode 재설정"));
        }, 0.1f, false);
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

