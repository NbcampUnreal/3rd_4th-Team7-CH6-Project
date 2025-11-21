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

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			LoadLevel(LobbyLevelName);
		},
		3.0f,
		false
	);
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
