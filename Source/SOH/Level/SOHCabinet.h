#pragma once

#include "CoreMinimal.h"
#include "Level/SOHOpenDoor.h"
#include "SOHCabinet.generated.h"

UCLASS()
class SOH_API ASOHCabinet : public ASOHOpenDoor
{
	GENERATED_BODY()
	
public:

	ASOHCabinet();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cabinet|Animation")
	UAnimMontage* HideEnterMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cabinet|Animation")
	UAnimMontage* HideExitMontage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cabinet|State")
	bool bIsHidden = false;

	virtual void Interact_Implementation(AActor* Caller) override;
};
