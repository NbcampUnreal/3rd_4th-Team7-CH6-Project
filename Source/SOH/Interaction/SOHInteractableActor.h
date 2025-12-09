
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHInteractInterface.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Interaction|UI")
	bool bEnableProximityText = true;

	UPROPERTY(EditAnywhere, Category = "Interaction|UI", meta = (EditCondition = "bEnableProximityText"))
	FVector2D WidgetSize = FVector2D(200.f, 50.f);

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	class UBoxComponent* InteractionRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|UI")
	class USceneComponent* UIAnchor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|UI")
	class UWidgetComponent* InteractionWidget;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
};
