#include "SOHGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ASOHGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FString MapName = GetWorld()->GetMapName();
	UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay: %s"), *MapName);
}

void ASOHGameModeBase::StartGame()
{
	UE_LOG(LogTemp, Log, TEXT("StartGame: Opening GameLevel"));
	LoadLevel(GameLevelName);
}

void ASOHGameModeBase::OnGameClear()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Clear!"));
	ShowGameClearUI();
	
}

void ASOHGameModeBase::OnPlayerDied()
{
	UE_LOG(LogTemp, Error, TEXT("Player Died!"));
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

void ASOHGameModeBase::LoadLevel(FName LevelName)
{
	if (!LevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, LevelName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadLevel: Invalid LevelName"));
	}
}
