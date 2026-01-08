#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "GameplayTagContainer.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "SOHComputer.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class UUserWidget;

UCLASS()
class SOH_API ASOHComputer 
	: public ASOHInteractableActor
	, public ISOHSaveObjectInterface
{
	GENERATED_BODY()
	
public:
	ASOHComputer();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	FGameplayTag PuzzleSolvedTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
	FName WorldStateID;
    
	virtual void SaveState_Implementation(USOHSaveGame* SaveData) override;
	virtual void LoadState_Implementation(USOHSaveGame* SaveData) override;
	
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
