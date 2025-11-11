// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ODObjectSlot.generated.h"

class UCheckBox;
class UTextBlock;
class UImage;
class UButton;
class UBorder;

DECLARE_DELEGATE_TwoParams(FOnObjectSlotClickedSignature,bool /*Current Selection State*/,int32/*Slot Index*/);
DECLARE_DELEGATE_TwoParams(FOnSelectItems,bool /*Current Selection State*/,TArray<int32>/*Slot Index*/);
DECLARE_DELEGATE_OneParam(FOnRemoveSelectedItems,TArray<int32>);
DECLARE_DELEGATE_TwoParams(FOnSlotDetailsVisibilityChanged,bool,TArray<int32>);

UCLASS()
class OBJECTDISTRIBUTION_API UODObjectSlot : public UUserWidget
{
	GENERATED_BODY()
	
	uint8 bSelectionState:1;
	uint8 ActivateStatus:1;
	int32 SlotIndex;
	FName PresetName;
	
public:
	UODObjectSlot(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	const int32& GetSlotIndex() const {return SlotIndex;}
	
	const FName& GetPresetName() const {return PresetName;}

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotSelectBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SpawnCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> PresetBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> CheckBoxBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> ActivateCheckBox;

	UFUNCTION()
	void SlotSelectBtnPressed();

	UFUNCTION()
	void OnActivateCheckBoxChanged(bool bInNewCondition);
	
public:
	void SetSlotParams(const FAssetData& AssetData,const bool InActivateStatus, const int32 InSlotIndex,const int32& InSpawnCount,const FName InOwnerPreset = FName(),const FLinearColor InPresetColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("000000FF"))));

	void SetSelectionState(bool InSelectionState);

	void CreateSlotThumbnail(const FAssetData& AssetData) const;

	void SetSpawnCount(const int32& InSpawnCount) const;

	void ChangeActivateStatus(const bool& InNewStatus);

	FOnObjectSlotClickedSignature OnObjectSlotClickedSignature;	
	FOnSelectItems OnSelectItems;
	FOnRemoveSelectedItems OnRemoveSelectedItems;
	FOnSlotDetailsVisibilityChanged OnSlotDetailsVisibilityChanged;
	
	void SetDetailsVisibility(const bool InbIsVisible);
	
private:
	TSharedRef<SWidget> GetSlotContextMenu();
	void ChangeSlotDetailsVisibility(const bool InbIsMouseEnter);
	void SetSpawnDensity(float InDensity);
	void SliderCaptureEnd();

private:
	//Double Click
	float LastClickTime = 0.0f;
	float LastDoubleClickTime = 0.0f;
};
