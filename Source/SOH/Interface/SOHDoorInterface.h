#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SOHDoorInterface.generated.h"

class ASOHAIMonster;

UINTERFACE(MinimalAPI)
class USOHDoorInterface : public UInterface
{
	GENERATED_BODY()
};

class SOH_API ISOHDoorInterface
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
    void OpenDoorForAI(ASOHAIMonster* Monster);
};
