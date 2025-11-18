
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHInteractInterface.h"
#include "Components/MeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "SOHInteractableActor.generated.h"

UCLASS()
class SOH_API ASOHInteractableActor : public AActor, public ISOHInteractInterface
{
	GENERATED_BODY()
	
public:	
	ASOHInteractableActor();

protected:

	virtual void Interact_Implementation(AActor* Caller) override;
	virtual bool CanReceiveTrace_Implementation(AActor* Caller, bool bCanInteract) override;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowInteractWidget();
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HideInteractWidget();
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ApplyOverlayMaterial(UMaterialInterface* Material);

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|UI")
	TSubclassOf<UUserWidget> InteractionWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> InteractionWidgetInstance;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Visual")
	TObjectPtr<UMaterialInterface> OutlineMaterial;
};
