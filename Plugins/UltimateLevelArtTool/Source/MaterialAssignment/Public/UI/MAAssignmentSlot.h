// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AssetRegistry/AssetData.h"
#include "MAAssignmentSlot.generated.h"

class UMAMaterialParamObject;
class USinglePropertyView;
class UButton;
class UTextBlock;
class UEditableTextBox;
class UPanelWidget;

UCLASS()
class MATERIALASSIGNMENT_API UMAAssignmentSlot : public UUserWidget
{
	GENERATED_BODY()

	FName SlotName;
	
	FString DefaultMaterialName;
	
	bool bMaterialParamChangedOnce;

	UPROPERTY()
	FAssetData SuggestedMaterialData;

public:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UPROPERTY()
	TObjectPtr<UMAMaterialParamObject> MaterialParamObject;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CounterText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> MultipleChoiceBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> SlotRenameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MeshNameList;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USinglePropertyView> MaterialPropertyView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuickAssignBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeleteBtn;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuickAssignText;
	
	UFUNCTION()
	void ApplyBtnPressed();

	UFUNCTION()
	void QuickAssignBtnPressed();

	UFUNCTION()
	void DeleteBtnPressed();

	UFUNCTION()
	void OnSlotNameTextCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	
private:
	TArray<FString> MeshNames;

	void RefreshTheSMNameList();
	
	void AssignSuggestedMaterial(const FAssetData& InAssetData);

public:
	void InitializeTheSlot(const FName& InSlotName,UMaterialInterface* InExistingMaterial, const FString& InFirstMeshName,const FAssetData& InSuggestedMaterial,bool IsSlotInUse);
	void AddNewObjectToCounter(const FString& InMeshName,bool IsSlotInUse);
	void CheckForMaterialDifferences(const UMaterialInterface* InMaterial) const;
	TObjectPtr<UMaterialInterface> GetNewMaterial() const;
	FName& GetSlotName(){return SlotName;}
	void DisableApplyButton();
	const bool& IsMaterialParamChanged() const {return bMaterialParamChangedOnce;}

	void AllMaterialsApplied();

protected:
	void OnMaterialParamChanged();
	
};
