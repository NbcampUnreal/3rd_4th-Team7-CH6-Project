// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "MBMBuildingSettingsObj.h"
#include "Editor.h"
#include "MBToolData.h"
#include "MBToolSubsystem.h"

void UMBMBuildingSettingsObj::SetupObjectSettings()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSubsystem || !ToolSubsystem->GetToolData()){return;}

	const FMBModBuildingSettingsData& BuildingSettingsData = ToolSubsystem->GetToolData()->BuildingSettingsData;
	bEnableModularSnapping = BuildingSettingsData.bEnableModularSnapping;
	bSnapOffsetCond = BuildingSettingsData.bSnapOffsetCond;
	SnapOffset = BuildingSettingsData.SnapOffset;
	bZOffsetCond = BuildingSettingsData.bZOffsetCond;
	ZOffset = BuildingSettingsData.ZOffset;
	SnappingType = BuildingSettingsData.SnappingType;
	BoundCorrectionSensitivity = BuildingSettingsData.BoundCorrectionSensitivity;
}

void UMBMBuildingSettingsObj::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSubsystem || !ToolSubsystem->GetToolData()){return;}

	FMBModBuildingSettingsData& BuildingSettingsData = ToolSubsystem->GetToolData()->BuildingSettingsData;
	BuildingSettingsData.bEnableModularSnapping = bEnableModularSnapping;
	BuildingSettingsData.bSnapOffsetCond = bSnapOffsetCond;
	BuildingSettingsData.SnapOffset = SnapOffset;
	BuildingSettingsData.bZOffsetCond = bZOffsetCond;
	BuildingSettingsData.ZOffset = ZOffset;
	BuildingSettingsData.SnappingType = SnappingType;
	BuildingSettingsData.BoundCorrectionSensitivity = BoundCorrectionSensitivity;

	if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		ToolSubsystem->SaveMBToolDataToDisk();
	}
}


