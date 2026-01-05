#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SOHSaveObjectInterface.generated.h"

class USOHSaveGame;

UINTERFACE(BlueprintType)
class SOH_API USOHSaveObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class SOH_API ISOHSaveObjectInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Save")
	void SaveState(class USOHSaveGame* SaveData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Save")
	void LoadState(class USOHSaveGame* SaveData);
};