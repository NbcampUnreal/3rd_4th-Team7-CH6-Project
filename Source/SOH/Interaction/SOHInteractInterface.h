#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SOHInteractInterface.generated.h"
class AActor;

UINTERFACE(MinimalAPI)
class USOHInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class SOH_API ISOHInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void Interact(AActor* Caller);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	bool CanReceiveTrace(AActor* Caller, bool bCanInteract);
};
