#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Item/SOHInventoryComponent.h"
#include "SOHSaveGame.generated.h"

UCLASS()
class SOH_API USOHSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
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
    
	UPROPERTY(BlueprintReadWrite)
	TArray<FSOHInventoryItem> SavedInventory;

	USOHSaveGame()
	   : SavedHealth(200.f)
	   , SavedStamina(100.f)
	   , SavedStage(0)
	{}
};
