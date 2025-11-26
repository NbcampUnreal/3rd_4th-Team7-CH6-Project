#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHComputer.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class UUserWidget;

UCLASS()
class SOH_API ASOHComputer : public ASOHInteractableActor
{
	GENERATED_BODY()
	
public:
	ASOHComputer();
	
protected:
	virtual void Interact_Implementation(AActor* Caller) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UUserWidget> ComputerWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Computer State")
	bool bIsPasswordSolved = false; 
	UFUNCTION(BlueprintCallable, Category = "Computer State")
	void SetPasswordSolved(bool bSolved);
private:
	void ShowComputerWidget();
	FTimerHandle TimerHandle;
};
