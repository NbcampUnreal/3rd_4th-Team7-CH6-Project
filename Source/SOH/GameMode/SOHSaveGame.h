#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "SOHSaveGame.generated.h"

UCLASS()
class SOH_API USOHSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FName SaveLevelName;

	UPROPERTY(BlueprintReadWrite)
	FTransform PlayerTransform;

	UPROPERTY(BlueprintReadWrite)
	float SavedHealth;

	UPROPERTY(BlueprintReadWrite)
	float SavedStamina;

	UPROPERTY(BlueprintReadWrite)
	int32 SavedStage;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer SavedConditions;

	USOHSaveGame()
	   : SavedHealth(100.f)
	   , SavedStamina(100.f)
	   , SavedStage(0)
	{}
};
