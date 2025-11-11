// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MBModularEnum.h"
#include "MBCategoryWindow.generated.h"

class UVerticalBox;
class USizeBox;
class UMBTypeWindow;
class UEditableTextBox;
class UButton;
class UTextBlock;

UCLASS()
class MODULARBUILDING_API UMBCategoryWindow : public UUserWidget
{
	GENERATED_BODY()
	

	UPROPERTY()
	TMap<FName,TObjectPtr<UMBTypeWindow>> TypeWindows;

	UPROPERTY()
	EBuildingCategory Category;

public:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoryMenuBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> MainCategoryBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> AddNewTypeText;

public:
	void RegenerateTheCategoryMenu(const EBuildingCategory BuildingCategory);

	void ResetSlotSelectionStates();

	UFUNCTION()
	void OnAddNewTypeTextCommitted(const FText& Text,ETextCommit::Type CommitMethod);

private:

	void AddASpacer();

	void CreateTypeMenus();

};

