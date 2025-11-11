// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ODDistributionBase.h"
#include "Data/ODMeshData.h"
#include "ODToolWindow.generated.h"

class UODPaletteSlate;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelActorDeletedNativeSignature, AActor*, Actor);

class UWrapBox;
class UBorder;
class AODSpawnCenter;
class UODPresetObject;
class AStaticMeshActor;
class UODDistributionBase;
class UButton;
class UEditableTextBox;
class UDetailsView;
class UPanelWidget;
class AActor;
class UODAssetBorder;
class UODSimulationWidget;
class UODPaletteDataObject;
class UODObjectSlot;
enum class EPaletteButtonType : uint8;

UCLASS()
class OBJECTDISTRIBUTION_API UODToolWindow : public UEditorUtilityWidget
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsToolDestroying = false;

	UPROPERTY()
	bool bTraceForVelocity = false;
	
	UPROPERTY()
	FVector LastSpawnCenterLocation;

	UPROPERTY()
	FVector SpawnCenterVelocity;

	UPROPERTY()
	bool bTraceForRotationDiff = false;
	
	UPROPERTY()
	FRotator LastSpawnCenterRotation;

	UPROPERTY()
	FRotator SpawnCenterRotation;

	bool IsMouseOnToolWindow = false;

public:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	void OnDistributionTypeChanged(EObjectDistributionType NewDistributionType);
	
	UPROPERTY()
	TObjectPtr<UODDistributionBase> PropDistributionBase;

	UPROPERTY()
	TObjectPtr<UODPresetObject> PresetObject;

	/*UPROPERTY()
	TObjectPtr<UODPaletteDataObject> PaletteDataObject;*/
	
	static FVector FindSpawnLocationForSpawnCenter();

	void GetSpawnCenterToCameraView() const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDetailsView> PresetDetails;
	
	// UPROPERTY(meta = (BindWidget))
	// TObjectPtr<UDetailsView> PaletteObjectDetails;

	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDetailsView> PropDistributionDetails;*/

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> PresetSettingsBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> TitleBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UODPaletteSlate> PaletteSlate;

	/*UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> PaletteBackgroundBtn;*/
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> FinishBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SimulationBorder;

	UPROPERTY()
	TObjectPtr<UODSimulationWidget> SimulationWidget;

private:
	void CreateSimulationWidget();

public:
#pragma region Reference


protected:
	UPROPERTY()
	TObjectPtr<AODSpawnCenter> SpawnCenterRef;
	
	void SpawnSpawnCenter();

public:
	AActor* GetSpawnCenterRef() const;

#pragma endregion Reference

	UPROPERTY(BlueprintAssignable)
	FOnLevelActorDeletedNativeSignature OnLevelActorDeletedNative;
	
	void HandleOnLevelActorDeletedNative(AActor* InActor);

protected:
	
#pragma region Presets

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddNewPresetBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddSelectedAssetsBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> NewPresetText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> AddAssetsText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RenamePresetBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> RenamePresetText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RemovePresetBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SavePresetBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveAsNewPresetBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> SaveAsText;


	
	UFUNCTION()
	void AddNewPresetBtnPressed();

	UFUNCTION()
	void AddSelectedAssetsBtnPressed();
	
	UFUNCTION()
	void RenamePresetBtnPressed();

	UFUNCTION()
	void RemovePresetBtnPressed();

	UFUNCTION()
	void SavePresetBtnPressed();

	UFUNCTION()
	void SaveAsNewPresetBtnPressed();

	UFUNCTION()
	void OnNewPresetTextCommitted( const FText& InText, ETextCommit::Type InCommitMethod);
	
	UFUNCTION()
	void OnAddAssetsTextCommitted( const FText& InText, ETextCommit::Type InCommitMethod);

	UFUNCTION()
	void OnRenamePresetTextCommitted( const FText& InText, ETextCommit::Type InCommitMethod);

	UFUNCTION()
	void OnSaveAsTextCommitted( const FText& InText, ETextCommit::Type InCommitMethod);

	UFUNCTION()
	void PaletteBackgroundBtnPressed();

	UFUNCTION()
	void FinishBtnPressed();

private:
	void OnFinishConditionChanged(bool InCanFinish);

	bool bAskOnce;
	float AskOnceTimer;

#pragma endregion Presets

#pragma  region  DistributionVisualiton

	void OnAfterODRegenerated() const;

#pragma  endregion  DistributionVisualiton

private:
	void OnMixerModeChanged(bool InIsInMixerMode);

	void OnPresetCategoryHidden(bool InbIsItOpen);

	void OnAssetsDropped(TArrayView<FAssetData> DroppedAssets);
	
	void OnVisualizationParamChanged();
	void OnPhysicsParamChanged();
	
	//Palette
	UFUNCTION()
	void OnPresetLoaded();

	UFUNCTION()
	void RebuildPalette();

	UFUNCTION()
	void OnObjectSlotPressed(const bool InFormalSelectionState,const int32 InSlotIndex);

	UPROPERTY()
	TArray<TObjectPtr<UODObjectSlot>> ObjectSlots;
	
	void ClearPalette();
public:
	void OnPaletteObjectParamChanged(const FName InParamName);
	
	void SimulationStoppedWithForcibly();
	
private:
	//Palette Slot Callbacks
	void OnSelectSlotItems(const bool InSelect, TArray<int32> InItemIndexes);
	void OnRemoveItemsFromPalette(const TArray<int32> InItemsToRemove);
	void OnSlotDetailsVisibilityChanged(const bool InbNewVisibility,const TArray<int32> InSlotIndexes);
	void OnAMixerPresetCheckStatusChanged(bool InNewCheckStatus, FName InPresetName);
	void OnObjectActivateStatusChanged(const bool InActivate, const TArray<int32> InIndex);
	void OnLocalSpawnDensityChanged();
	
	void OnPaletteButtonPressed(const EPaletteButtonType InPaletteButtonType);
	
	void NotifyPaletteSlotSelectionChanges() const;

	//Simulation Callbacks
	void OnStartSimClicked();
	void OnStopSimClicked();

	void HandleBeginPIE(const bool bIsSimulating);
	void HandleEndPIE(const bool bIsSimulating);
	void HandlePausePIE(bool bIsSimulating);
	void HandleResumePIE(bool bIsSimulating);

	void OnToolReset();
	
	void OnTotalSpawnCountChanged(const float InSpawnCount);

	void OnPaletteBackgroundPressed();
	
#pragma region  InputPreProcessor

	void ActivateODInputProcessor();

	void DeactivateODInputProcessor() const;

	TSharedPtr<class FODKeyInputPreProcessor> ODInputProcessor;
	
	bool bKeySelected = false;
	bool bIsUserSettingsOn = false;

protected:
	bool HandleKeySelected(FKey InKey);
	bool HandleKeySelectionCanceled(FKey InKey);
#pragma endregion  InputPreProcessor

#pragma region  InputData
	bool bIsLeftShiftPressed = false;
	bool bIsLeftCtrlPressed = false;
#pragma endregion  InputData

	
};
