// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetData.h"
#include "Interfaces/MAToolWindowInterface.h"
#include "MAToolWindow.generated.h"

class UScrollBox;
class UMAAssignmentSlot;
class UODPresetObject;
class UODSpawnCenterData;
class AStaticMeshActor;
class UODDistributionBase;
class UButton;
class AActor;
class UStaticMesh;
class UScrollBox;
class UPanelWidget;
class UWidgetAnimation;

UCLASS()
class MATERIALASSIGNMENT_API UMAToolWindow : public UEditorUtilityWidget, public IMAToolWindowInterface
{
	GENERATED_BODY()

	bool bMouseOnWToolWindow;

	UPROPERTY()
	TArray<FName> SlotNames;

	UPROPERTY()
	TArray<UStaticMesh*> SelectedStaticMeshes;

	UPROPERTY()
	TArray<UMAAssignmentSlot*> MaterialSlotWidgets;
	
	int32 AlteredSlotCounter;

	FARFilter Filter;
	
public:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyAllBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> SlotBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ApplyAllBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NoMeshText;

	UPROPERTY( Transient, meta = ( BindWidgetAnim ) )
	TObjectPtr<UWidgetAnimation> SelectMeshTextAnim;
	
	UFUNCTION()
	void ApplyAllBtnPressed();

	static bool SaveModifiedAssets(const TArray<UPackage*>& InPackages);

private:
	void UpdateSelectedAssets();
	
	void ResetAllVariables();

	static TArray<UStaticMesh*> FilterStaticMeshAssets(const TArray<UObject*>& Assets);

	void RecalculateApplyAllBtnStatus();

public:
	//INTERFACE
	virtual void MaterialPropertyChanged(bool bIncreaseCounter) override;
	virtual void ApplySingleMaterialChange(const FName& SlotName , UMaterialInterface* InMaterialInterface) override;
	virtual bool RenameMaterialSlot(const FName& CurrentSlotName,const FName& NewSlotName) override;
	virtual void DeleteSlot(const FName& InSlotName) override;
	
	void HandleOnAssetSelectionChanged(const TArray<FAssetData>& NewSelectedAssets, bool bIsPrimaryBrowser);
	void HandleOnAssetPathChanged(const FString& NewPath);
	void HandleOnAssetRemoved(const FAssetData& InAsset);
	void HandleOnAssetRenamed(const FAssetData& InAsset, const FString& InOldObjectPath);
	
private:
	void FixUpTheList();
	void FixUpDirectories() const;
};
