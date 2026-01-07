#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHCutscenePlayerBase.generated.h"

UCLASS(Abstract)
class SOH_API ACutscenePlayerBase : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Cutscene")
	void PlayCutscene();
};

