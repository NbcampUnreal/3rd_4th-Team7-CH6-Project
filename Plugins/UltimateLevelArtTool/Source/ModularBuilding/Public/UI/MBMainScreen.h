// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Interfaces/MBMainScreenInterface.h"
#include "Styling/SlateTypes.h"
#include "MBMainScreen.generated.h"

class UMBCollectionButton;
class UMBSettingMenu;
class UMBToolSubsystem;
class UButton;
class USizeBox;
class UPanelWidget;
class UMBBuildingManager;
class UMBCategoryWindow;
struct FDuplicationData;
class UTextBlock;

UCLASS()
class MODULARBUILDING_API UMBMainScreen : public UEditorUtilityWidget, public IMBMainScreenInterface
{
	GENERATED_BODY()

	bool bCanWindowTick = true;
	
#pragma region  InputPreProcessor

private:
	void ActivateInputProcessor();
	void DeactivateInputProcessor() const;
	
	bool bKeySelected = false;
	bool bIsUserSettingsOn = false;
	TSharedPtr<class FMBKeyInputPreProcessor> InputProcessor;
	
	void CreateAndInitializeTheBuildingManager();
	
	UPROPERTY()
	TObjectPtr<UMBBuildingManager> BuildingManager;
	
	UPROPERTY()
	TArray<TObjectPtr<UMBCollectionButton>> CollectionButtons;

	const float CancelTimeDelay = 0.1f;
	
	bool bIsAnAssetChanged = false;


protected:
	bool HandleKeySelected(FKey InKey);
	bool HandleKeySelectionCanceled(FKey InKey);

	void HandleOnAssetRenamed(const FAssetData& InAsset, const FString& InOldObjectPath);
	void HandleOnAssetUpdated(const FAssetData& InAsset);

#pragma endregion  InputPreProcessor

private:
	void InitializeTheAssetMenu();

	void RegenerateCollectionBox();

	void StartSyncingAssetOnBrowser() const;

	void SelectCollectionPivot() const;
	
	UPROPERTY()
	TObjectPtr<UMBToolSubsystem> ToolSettingsSubsystem;
	
	UPROPERTY()
	TObjectPtr<UMBSettingMenu> SettingsMenu;
	
	UPROPERTY()
	TObjectPtr<UMBCategoryWindow> CategoryMenu;

	void SetupBindings();

	void ResetLastAssetSelectionFromMemory() const;

protected:
	UPROPERTY(EditDefaultsOnly,Category="Modular Building")
	FButtonStyle CategorySelectedBtnStyle;

	UPROPERTY(EditDefaultsOnly,Category="Modular Building")
	FButtonStyle CategoryNotSelectedBtnStyle;
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeartesBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> MainContentBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SettingsBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> CollectionBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MultiplePlacementText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ModularCategoryBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PropCategoryBtn;
	
	UFUNCTION()
	void ModularCategoryBtnPressed();

	UFUNCTION()
	void PropCategoryBtnPressed();
	
	UFUNCTION()
	void SettingsBtnPressed();

	UFUNCTION()
	void LeartesBtnPressed();
	
	void OpenSettingsMenu(const bool bInOpen);

	
protected:
	virtual void NativeOnMouseEnter( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent )override;
	
#pragma region Interface
	
public:
	FORCEINLINE virtual UObject* GetBuildingManager() override;
	FORCEINLINE virtual void ChangeTheCategory() override;
	FORCEINLINE virtual void CreateAssetClicked(const FString AssetName) override;
	FORCEINLINE virtual void StartModularDuplicationFromSettings(const FDuplicationData& InDuplicationData) override;	
	FORCEINLINE virtual TArray<FDuplicationData*> GetExistingDuplicationData() override;
	FORCEINLINE virtual void ApplyModularDuplicationPressed() override;
	FORCEINLINE virtual void ApplyModularDuplicationFilter(const FDuplicationFilters& InDuplicationFilters) override;
	FORCEINLINE virtual FDuplicationFilters* GetExistingDuplicationFilter() override;
	FORCEINLINE virtual void UpdateTheCollectionTabState() override;
	FORCEINLINE virtual void UpdateCategoryWindow() override;
	FORCEINLINE virtual void SetMultiplePlacementAmountText(const FString& InText) const override;
	FORCEINLINE virtual void ResetToolWindowInterface() override;
	
#pragma endregion Interface

public:
	FORCEINLINE virtual void AddNewCollection() override;
	FORCEINLINE virtual void TurnOffCollectionState(const int32 InIndex) override;

	FORCEINLINE virtual void AssetTypeRemoved() override;

	FORCEINLINE virtual void CollectionRemoved() override;
	FORCEINLINE virtual void CollectionRestored() override;
	FORCEINLINE virtual void CollectionNameChanged(const FName& InOldName,const FName& InNewName) override;
#if WITH_EDITOR
	void OnSelectionChanged(UObject* Object);

#endif
	
	UFUNCTION()
	void OnNewAssetsAddedToTool();

private:
	void AddFreePlacementDistance(const bool bIsAdding) const;

#pragma region MouseMovements

private:
	bool bListeningForAssetDoubleClickEvent = false;
	
	float AssetDoubleClickCounter = 0;
	
	FString AssetToCreate;

	void StartAssetDoubleClickListener();
	void CreateAssetOneClickEvent(const FString& InAssetName);
	void CreateAssetDoubleClickClickEvent();
	void ResetDoubleClickVariables();
	

#pragma endregion MouseMovements

private:
	virtual void OnEditorShutdown();

	void EjectAllBindings();

	
}; 
