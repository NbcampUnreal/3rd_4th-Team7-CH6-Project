// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "MBModularEnum.h"
#include "MBToolSubsystem.generated.h"

class UMBUserSettings;
class UStaticMesh;
class UDataTable;
struct FModularBuildingAssetData;
class UMBToolData;
class AGroupActor;
class AStaticMeshActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlacementTypeChanged);

UCLASS(BlueprintType)
class MODULARBUILDING_API UMBToolSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LoadDataAssets();
	
	virtual void Deinitialize() override;

private:

	UPROPERTY()
	TObjectPtr<UMBToolData> ToolData;
	
	UPROPERTY()
	TObjectPtr<UMBUserSettings> ToolUserSettings;

	UPROPERTY()
	TObjectPtr<UDataTable> ModularAssetData;	

	UPROPERTY()
	TWeakObjectPtr<UObject> MBToolMainScreen;
	
	void CheckForNullModularDataAndRemove() const;

	
public:
	FORCEINLINE UMBToolData* GetToolData() const {return ToolData;}
	FORCEINLINE UMBUserSettings* GetToolUserSettings() const {return ToolUserSettings;}
	FORCEINLINE UDataTable* GetModularAssetData() const {return ModularAssetData;}

	FORCEINLINE void SetMBToolMainScreen(UObject* InMainScreenPtr) {MBToolMainScreen = InMainScreenPtr;}
	FORCEINLINE TWeakObjectPtr<UObject> GetToolMainScreen() const {return MBToolMainScreen;}
	FORCEINLINE void ReleaseMBToolMainScreenRef() {MBToolMainScreen = nullptr;}

	bool AddNewMeshType(const FName InTypeName,const EBuildingCategory InCategory) const;
	bool ChangeAssetType(const FName InOldTypeName,const FName InNewTypeName,const EBuildingCategory InCategory);
	bool RemoveAssetType(const FName InTypeName,const EBuildingCategory InCategory) const;
	bool ChangeTypeOrder(const FName InTypeName,const EBuildingCategory InCategory,bool bInIsGoingUp) const;
	bool RemoveSameTypedAssetsFromTool(const FName& InTypeName,const EBuildingCategory& InCategory) const;

	void RemoveAssetByName(const FString& InName) const;
	
	//Collection Actions
	int32 CreateANewCollection() const;
	static FName CreteAUniqueCollectionName(const TArray<FName>& InCollections);
	bool ChangeCollectionName(const FName InOldName,const FName InNewName) const;
	void CreateAssetFromCurrentCollection() const;
	void BatchCurrentCollection(EMBMeshConversionType InConversionType) const;
	void RunMergeToolForCurrentCollection() const;
	bool SelectAllInCollection() const;
	bool DeleteCurrentCollection();

	const FModularBuildingAssetData* GetModAssetRowWithAssetName(const FString& InName) const;

	void SyncAssetInBrowser(const FString& InAssetName) const;
	void OpenAsset(const FString& InAssetName) const;

protected:
	void HandleOnAssetRemoved(const FAssetData& InAsset) const;
	void HandleOnAssetRenamed(const FAssetData& InAsset, const FString& InOldObjectPath) const;

private:
	UBlueprint* HarvestBlueprintFromActors(const FName BlueprintName, UPackage* Package, const TArray<AActor*>& Actors) const;
public:
	UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ToolSettings")
	FOnPlacementTypeChanged OnNewAssetsAdded;
	
	UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ToolSettings")
	FOnPlacementTypeChanged OnPlacementTypeChanged;
	
	void AddNewAssetsToTheTool(FName InAssetType,TArrayView<FAssetData> InDroppedAssets) const;
	FModularBuildingAssetData* GetModularAssetDataByName(const FString& InAssetName) const;
	
	FName GetActiveCollectionFolderPath() const;

	
	void OpenModularBuildingSettingsDataAsset() const;

	void DeleteAllModularActorOnTheLevel();

	void ChangePlacementType() const;

	UPROPERTY()
	bool IsTheSettingsOn = false;

	UPROPERTY()
	EPlacementMode PlacementMode = EPlacementMode::None;

	UPROPERTY()
	bool bIsMouseOnToolWindow = false;
	
	UPROPERTY()
	EBuildingCategory MovedAssetCategory = EBuildingCategory::None;

	UPROPERTY()
	EMBWorkingMode WorkingMode = EMBWorkingMode::None;
	
	UPROPERTY()
	bool bIsDuplicationInprogress = false;

	UPROPERTY()
	int32  LastActiveCollectionIndex = -1;
	
	UPROPERTY()
	FName ActiveCollectionWindow = FName();

	//Input Modifiers
	UPROPERTY()
	bool bIsShiftPressed = false;

	UPROPERTY()
	bool bIsCtrlPressed = false;

	UPROPERTY()
	FString LastCreatedAsset = FString();
	
	void ResetTool();

	//Save Data 
	void SaveModularAssetDataToDisk() const;

	void SaveMBToolDataToDisk() const;
	
	void SaveAllModularData() const;

public:
	void RemoveNullAssetRow(const FName& InRowName) const;
	
};
