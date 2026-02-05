#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Item/SOHInventoryComponent.h"
#include "SOHSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FWorldStateData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsOpen = false;

	UPROPERTY()
	bool bIsOn = false;

	UPROPERTY()
	bool bIsSolved = false;
	
	UPROPERTY()
	bool bIsLocked = false; 
	
	UPROPERTY()
	bool bIsCollected = false;
	
	UPROPERTY()
	int32 IntValue = 0;
};

UCLASS()
class SOH_API USOHSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bHasFlashlight = false;
	
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
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FWorldStateData> WorldStateMap;

	USOHSaveGame()
	   : SavedHealth(200.f)
	   , SavedStamina(100.f)
	   , SavedStage(0)
	{}
};

