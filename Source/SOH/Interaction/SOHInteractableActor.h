
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHInteractInterface.h"
#include "Components/WidgetComponent.h"
#include "SOHInteractableActor.generated.h"

class UMaterialInterface;
class UStaticMeshComponent;
class UUserWidget;
class UWidgetAnimation;


UCLASS()
class SOH_API ASOHInteractableActor : public AActor, public ISOHInteractInterface
{
	GENERATED_BODY()
	
public:	
	ASOHInteractableActor();
	void BeginPlay() override;

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
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Visual")
	UMaterialInterface* OutlineMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	UWidgetComponent* InteractionWidget;
};
