#pragma once

#include "CoreMinimal.h"
#include "Item/SOHBaseItem.h"
#include "SOHBattery.generated.h"

UCLASS()
class SOH_API ASOHBattery : public ASOHBaseItem
{
	GENERATED_BODY()

public:
	ASOHBattery();

	UFUNCTION(BlueprintCallable)
	bool Use(AActor* User, const FSOHItemTableRow& ItemData);

protected:
	virtual void Interact_Implementation(AActor* Caller) override;
};
