// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBToolSubsystem.h"
#include "Engine/DataAsset.h"
#include "Editor.h" 
#include "MBMainScreenInterface.h"
#include "MBToolData.generated.h"


USTRUCT(BlueprintType)
struct FMBModBuildingSettingsData
{
	GENERATED_BODY()
	
	FORCEINLINE FMBModBuildingSettingsData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	bool bEnableModularSnapping = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	bool bSnapOffsetCond = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	float SnapOffset = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	bool bZOffsetCond = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	float ZOffset = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	EMBModularSnapType SnappingType = EMBModularSnapType::BoundCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Modular Building")
	float BoundCorrectionSensitivity = 50.0f;

	EPlacementType PlacementType = EPlacementType::Single;
};

FMBModBuildingSettingsData::FMBModBuildingSettingsData()
{
	
}

USTRUCT(BlueprintType)
struct FMBPropBuildingSettingsData
{
	GENERATED_BODY()
	
	FORCEINLINE FMBPropBuildingSettingsData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	bool bEnableSurfaceSnapping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	bool bCustomRotationRateCond = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	float CustomRotationRate = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	bool bScaleRateCond = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	float ScaleRate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	bool bZOffsetCond = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Prop Building")
	float ZOffset = 250.0f;
};

FMBPropBuildingSettingsData::FMBPropBuildingSettingsData()
{
	
}


UCLASS(BlueprintType)
class MODULARBUILDING_API UMBToolData : public UDataAsset
{
	GENERATED_BODY()
public:
	virtual void PostEditUndo() override;

	void ResetToDefault();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	TArray<FName> ModularMeshTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	TArray<FName> PropMeshTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	TArray<FName> Collections;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	EBuildingCategory LastActiveBuildingCategory = EBuildingCategory::Modular;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	FMBModBuildingSettingsData  BuildingSettingsData = FMBModBuildingSettingsData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Settings")
	FMBPropBuildingSettingsData  PropBuildingSettingsData = FMBPropBuildingSettingsData();
};

inline void UMBToolData::PostEditUndo()
{
	Super::PostEditUndo();

	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->GetToolMainScreen().Get())
		{
			Cast<IMBMainScreenInterface>(ToolSettingsSubsystem->GetToolMainScreen().Get())->CollectionRestored();
		}
	}	
}

inline void UMBToolData::ResetToDefault()
{
	Collections.Empty();
	Collections.Empty();
	ModularMeshTypes.Empty();
	ModularMeshTypes.Add(FName("Wall"));
	ModularMeshTypes.Add(FName("Corner"));
	ModularMeshTypes.Add(FName("Door"));
	ModularMeshTypes.Add(FName("Floor"));
	ModularMeshTypes.Add(FName("Roof"));
	
	PropMeshTypes.Empty();
	PropMeshTypes.Add(FName("Default Props"));

	LastActiveBuildingCategory = EBuildingCategory::Modular;
	
	BuildingSettingsData = FMBModBuildingSettingsData();
}
