// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "ODMeshData.h"
#include "ODToolSubsystem.generated.h"

class FReply;
class UODToolSettings;
struct FODPresetData;
struct FPresetMixerMapData;
class UDataTable;
class AStaticMeshActor;
struct FDistObjectData;
struct FLocalSpawnDensity;

DECLARE_MULTICAST_DELEGATE(FOnPresetLoaded);
DECLARE_DELEGATE_TwoParams(FOnObjectActivateStatusChanged,bool,TArray<int32>);
DECLARE_DELEGATE_OneParam(FOnMixerModeChanged, bool)
DECLARE_DELEGATE_TwoParams(FOnAMixerPresetCheckStatusChanged, bool,FName)
DECLARE_DELEGATE(FOnLocalDensityChanged);
DECLARE_DELEGATE(FOnToolReset);
DECLARE_DELEGATE(FOnVisualizationParamChanged);
DECLARE_DELEGATE(FOnPhysicsParamChanged);

UCLASS()
class OBJECTDISTRIBUTION_API UODToolSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UODToolSettings> ODToolSettings;
	
	UPROPERTY()
	TObjectPtr<UDataTable> PresetData;

	UPROPERTY()
	TMap<FName,FLinearColor> PresetColorMap;

	TSharedPtr<class SWindow> ToolSettingWindow;

	UPROPERTY()
	TObjectPtr<class UODSettingsObject> SettingsObject;

public:
	//If true simulating in progress but should be paused
	bool bIsSimulationInProgress;

	//If true simulating right now
	bool bIsSimulating;
	
	void RegeneratePresetColorMap();

	FLinearColor GetColorOfPreset(const FName& InPresetName);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LoadDataAssets();
	
	virtual void Deinitialize() override;

	void ToolWindowClosed();

	void SpawnSettingsMenu();

private:
	FReply OnResetToolPressed();

public:
	FOnToolReset OnToolReset;

	void SettingsMenuParamChanged(FName InParamName);
	
	FOnVisualizationParamChanged OnVisualizationParamChanged;
	FOnPhysicsParamChanged OnPhysicsParamChanged;

#pragma  region Preset
	
private:
	FName LastSelectedPreset;

public:
	TArray<FString> GetPresetNames() const;
	
	FName GetLastSelectedPreset() const {return LastSelectedPreset;}

	TObjectPtr<UODToolSettings> GetODToolSettings() const {return ODToolSettings;}
	
	bool IsPresetAvailable(const FString& InPResetName) const;
	
	FODPresetData* GetPresetData(const FName& InPResetName) const;
	
	void AddNewPreset(const FString& InPResetName);

	bool CreateNewPresetFromSelectedAssets(const FString& InPResetName) const;
	
	void SetActivePreset(const FString& InPResetName);
	
	void SetLastPresetAsActivePreset();
	
	void RenameCurrentPreset(const FString& InNewName);

	bool SaveCurrentPreset() const;

	void RemoveCurrentPreset();

	void LoadActivePreset();

	FOnPresetLoaded OnPresetLoaded;
private:
	void SavePresetDataToDisk() const;

	void RefreshThePresetDataTableForInvalidData() const;

#pragma  endregion Presets

#pragma  region PresetMixer

private:
	//This data updated automatically when a preset checked or unchecked
	UPROPERTY()
	TArray<FPresetMixerMapData> PresetMixerMapData;
	
public:
	void UpdateMixerCheckListAndResetAllCheckStatus();

	TArray<FPresetMixerMapData>& GetPresetMixerMapData(){return PresetMixerMapData;}

	bool bInMixerMode = false;
	
	void ToggleMixerMode();
	
	bool IsInMixerMode() const {return bInMixerMode;}
	
	FOnMixerModeChanged OnMixerModeChanged;
	FOnAMixerPresetCheckStatusChanged OnAMixerPresetCheckStatusChanged;

public:
	void MixerPresetChecked(const FPresetMixerMapData& InMixerMapData);
	void RegeneratePresetMixerDataFromScratch();
	
#pragma  endregion PresetMixer
	
#pragma region Distribution
public:
	
	inline  static int32 FolderNumberContainer = 1;
	
	FORCEINLINE static int32 GetUniqueFolderNumber() {return FolderNumberContainer++;}

	UPROPERTY()
	TArray<TObjectPtr<AStaticMeshActor>> CreatedDistObjects;

	UPROPERTY()
	TMap<int32,FDistObjectData> ObjectDataMap;

	UPROPERTY()
	TArray<FDistObjectData> ObjectDistributionData;
	
	UPROPERTY()
	TArray<FVector> InitialRelativeLocations;
	
	UPROPERTY()
	TArray<FRotator> InitialRelativeRotations;

	void DestroyKillZActors(const TArray<FName>& InActorNames);

	void CreateInstanceFromDistribution(const EODMeshConversionType& InTargetType);

private:
	static FBox GetAllActorsTransportPointWithExtents(const TArray<AStaticMeshActor*>& InActors);
	
#pragma endregion Distribution

#pragma region Palette

public:
	bool AddAssetsToPalette(const TArrayView<FAssetData>& DroppedAssets);

	TArray<int32> GetIndexesOfPreset(const FName& InPresetName);
	
	void RemoveSelectedAssetsFromPalette();

private:
	void CheckForSelectedPresetAvailabilityOnPalette();

public:

	void SyncPaletteItems(TArray<int32> InItems);
	void OpenPaletteItemOnSMEditor(const int32& InItem);
	
	TArray<FDistObjectData*> GetSelectedDistObjectData();
	TArray<FDistObjectData*> GetDistObjectDataWithIndexes(const TArray<int32>& InIndexes);

	void ActivateItemsWithIndex(bool InActivate,const TArray<int32>& InIndex);

	FOnObjectActivateStatusChanged OnObjectActivateStatusChanged;

	UPROPERTY()
	TArray<int32> SelectedPaletteSlotIndexes;

	UPROPERTY()
	int32 LastSelectIndex = -1;
	

	//Density
	// void StartDensitySession();
	// void StopDensitySession();
	void RestartDensitySession();
	void ChangeSlotObjectDensity(const float& InValue);
	TSharedPtr<FLocalSpawnDensity> LocalSpawnDensity;
	FOnLocalDensityChanged OnLocalDensityChanged;
private:
	UPROPERTY()
	int32 InitialTotal = 0.0f;

	UPROPERTY()
	TMap<int32,int32> DensityMap = {};

public:
	template <typename  T>
	static bool IsSoftObjectValidToUse(const TSoftObjectPtr<T>& InSMSoftObjectPtr)
	{
		if(InSMSoftObjectPtr.IsNull()){return false;}

		if(InSMSoftObjectPtr.IsPending())
		{
			return true;
		}
		if(InSMSoftObjectPtr.IsValid())
		{
			return true;
		}
	
		return false;
	}
	
#pragma endregion Palette
	
};



