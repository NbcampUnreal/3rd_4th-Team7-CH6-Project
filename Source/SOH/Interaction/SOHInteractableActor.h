
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHInteractInterface.h"
#include "SOHInteractableActor.generated.h"

UCLASS()
class SOH_API ASOHInteractableActor : public AActor, public ISOHInteractInterface
{
	GENERATED_BODY()
	
public:	
	ASOHInteractableActor();

protected:
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Caller) override;
	virtual bool CanReceiveTrace_Implementation(AActor* Caller) override;

};
