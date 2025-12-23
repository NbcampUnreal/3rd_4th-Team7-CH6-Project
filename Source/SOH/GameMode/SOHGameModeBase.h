#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SOHGameModeBase.generated.h"

UCLASS()
class SOH_API ASOHGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Called when game starts (UI에서 호출)
	UFUNCTION(BlueprintCallable, Category="Game")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ContinueGame();
	
	// Called when game is cleared
	UFUNCTION(BlueprintCallable, Category="Game")
	void OnGameClear();

	// Called when player dies
	UFUNCTION(BlueprintCallable, Category="Game")
	void OnPlayerDied();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> DeathWidgetClass;

	UPROPERTY()
	UUserWidget* DeathWidgetInstance;

protected:
	virtual void BeginPlay() override;
	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// Blueprint에서 UI 처리할 수 있도록 이벤트 선언
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void ShowGameOverUI();

	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void ShowGameClearUI();

private:
	// 공통 레벨 로딩 함수
	void LoadLevel(FName LevelName);
	
	UPROPERTY(EditDefaultsOnly, Category="Level")
	FName LobbyLevelName = "MainMenuLevel";

	UPROPERTY(EditDefaultsOnly, Category="Level")
	FName GameLevelName = "MainLevel";
	
	UPROPERTY(EditDefaultsOnly, Category="SaveGame")
	bool bEnableAutoSave = true;
	
};
