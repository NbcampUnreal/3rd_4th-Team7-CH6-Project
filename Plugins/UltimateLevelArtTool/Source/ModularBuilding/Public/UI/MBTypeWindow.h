// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Slate/MBAssetBorder.h"
#include "MBTypeWindow.generated.h"

class UPanelWidget;
class UScrollBox;
class UMBAssetSlot;
class UStaticMesh;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UWidget;
class UMBAssetSlot;

UCLASS()
class MODULARBUILDING_API UMBTypeWindow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	FName MenuType;

	UPROPERTY()
	TArray<UMBAssetSlot*> AddedSlots;
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	FORCEINLINE void SetMenuType(const FName& InType);
	FORCEINLINE const FName* GetMenuType() const {return &MenuType;}
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> AssetBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TitleTextBox;	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMBAssetBorder> AssetDropBorder;

	UPROPERTY(meta = (BindWidget , ToolTip))
	TObjectPtr<UButton> TypeUpBtn;
	
	UPROPERTY(meta = (BindWidget , ToolTip))
	TObjectPtr<UButton> TypeDownBtn;
	
	UPROPERTY(meta = (BindWidget , ToolTip))
	TObjectPtr<UButton> DeleteBtn;

public:
	
	void AddSlotToMenu(const UStaticMesh* InAsset);

	void OnAssetDropped(TArrayView<FAssetData> DroppedAssets) const;

	UFUNCTION()
	void OnTitleTextBoxCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void TypeUpBtnPressed();
	
	UFUNCTION()
	void TypeDownBtnPressed();
	
	UFUNCTION()
	void DeleteBtnPressed();

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void ResetAllSelectionStates();
};
