#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHComputer.generated.h"


UCLASS()
class SOH_API ASOHComputer : public ASOHInteractableActor
{
	GENERATED_BODY()
protected:

	virtual void Interact_Implementation(AActor* Caller) override;
};
