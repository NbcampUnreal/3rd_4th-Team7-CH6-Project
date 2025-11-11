// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MBModularEnum.h"
#include "MBCollectionWindow.generated.h"

class UMBToolSubsystem;
class USinglePropertyView;
class UMBCollectionDetailsObject;
class UMBMaterialAssignmentWindow;
class UButton;
class UPanelWidget;
class UEditableTextBox;

UCLASS()
class MODULARBUILDING_API UMBCollectionWindow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UMBToolSubsystem> ToolSettingsSubsystem;

	UPROPERTY()
	EBuildingCategory TabCategory = EBuildingCategory::Modular;
	
	UPROPERTY()
	TArray<UStaticMesh*> ModularStaticMeshes;

	UPROPERTY()
	TArray<UStaticMesh*> PropStaticMeshes;

	UPROPERTY()
	TObjectPtr<UMBMaterialAssignmentWindow> MaterialAssignmentWindow;
public:
	virtual void NativePreConstruct() override;

	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	void RegenerateTheSettingsMenu();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ModularTabBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ModularSlotBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> PropTabBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> PropSlotBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> MaterialAssignmentBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectAllBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectCollectionTransportBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateBlueprintBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MergeCollectionBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SingleConvertToBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeleteCollectionBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> CollectionTitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USinglePropertyView> SingleBatchConversionView;
	
	void RegenerateTypeSelectionTab();

	UPROPERTY(EditAnywhere,Category="Modular Building Tool",meta=(NoResetToDefault))
	EMBMeshConversionType TargetType;
	

public:
	UFUNCTION()
	void SelectAllBtnPressed();
	
	UFUNCTION()
	void SelectCollectionTransportBtnPressed();
	
	UFUNCTION()
	void CreateBlueprintBtnPressed();

	UFUNCTION()
	void MergeCollectionBtnPressed();

	UFUNCTION()
	void SingleConvertToBtnPressed();

	UFUNCTION()
	void DeleteCollectionBtnPressed();
	
	UFUNCTION()
	void OnActorSlotPressed(int32 InIndex,uint8 InSection);
private:
	UPROPERTY()
	TObjectPtr<AActor> TransportPoint;

	TObjectPtr<AActor> CreateTransportPoint();
	FVector ReCalculateTransportPoint() const;

	TArray<AActor*> GetModularCategoryActors() const;
	TArray<AActor*> GetPropCategoryActors() const;
	
public:
	void HandleOnActorSelectionChanged(const TArray<UObject*>& NewSelection, bool bForceRefresh);

	void HandleOnLevelActorDeletedNative(AActor* Actor);

	UFUNCTION()
	void OnTitleTextBoxCommitted(const FText& Text,ETextCommit::Type CommitMethod);

};
