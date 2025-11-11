// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBCollectionButton.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class MODULARBUILDING_API UMBCollectionButton : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName CollectionName;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void SetCollection(const FName InName);

	UFUNCTION()
	void SetSelectionState(const bool InSelectionState) const;
	
	FORCEINLINE const FName& GetCollectionName() const { return CollectionName; }
	FORCEINLINE void ChangeCollectionName(const FName& InCollectionName);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ModularCollectionBtn;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CollectionText;
	
	UFUNCTION()
	void ModularCollectionBtnPressed();

#pragma  region  DoubleClick
	
private:
	FTimerHandle DoubleClickTimerHandle;

	bool bIsCheckingInProgress;

	void CheckForDoubleClick();

	UFUNCTION()
	void OnTimerFinished();

	void OnDoubleClickCheckingFinished(const bool bInIsDoubleClicked);

#pragma endregion DoubleClick
};
