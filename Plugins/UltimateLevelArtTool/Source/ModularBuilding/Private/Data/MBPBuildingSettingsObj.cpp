// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "MBPBuildingSettingsObj.h"
#include "Editor.h"
#include "MBBuildingManagerInterface.h"
#include "MBToolData.h"
#include "MBToolSubsystem.h"

void UMBPBuildingSettingsObj::SetupObjectSettings()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSubsystem || !ToolSubsystem->GetToolData()){return;}

	const FMBPropBuildingSettingsData& PropBuildingSettingsData = ToolSubsystem->GetToolData()->PropBuildingSettingsData;
	bEnableSurfaceSnapping = PropBuildingSettingsData.bEnableSurfaceSnapping;
	CustomRotationRate = PropBuildingSettingsData.CustomRotationRate;
	bCustomRotationRateCond = PropBuildingSettingsData.bCustomRotationRateCond;
	bScaleRateCond = PropBuildingSettingsData.bScaleRateCond;
	ScaleRate = PropBuildingSettingsData.ScaleRate;
	bZOffsetCond = PropBuildingSettingsData.bZOffsetCond;
	ZOffset = PropBuildingSettingsData.ZOffset;
}

void UMBPBuildingSettingsObj::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSubsystem || !ToolSubsystem->GetToolData()){return;}

	FMBPropBuildingSettingsData& PropBuildingSettingsData = ToolSubsystem->GetToolData()->PropBuildingSettingsData;
	PropBuildingSettingsData.bEnableSurfaceSnapping = bEnableSurfaceSnapping;
	PropBuildingSettingsData.CustomRotationRate = CustomRotationRate;
	PropBuildingSettingsData.bCustomRotationRateCond = bCustomRotationRateCond;
	PropBuildingSettingsData.bScaleRateCond = bScaleRateCond;
	PropBuildingSettingsData.ScaleRate = ScaleRate;
	PropBuildingSettingsData.bZOffsetCond = bZOffsetCond;
	PropBuildingSettingsData.ZOffset = ZOffset;
	
	if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		ToolSubsystem->SaveMBToolDataToDisk();
	}

	if(PropertyChangedEvent.GetPropertyName() == TEXT("bEnableSurfaceSnapping"))
	{
		if(!GEditor){return;}
		if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
		{
			ToolSettingsSubsystem->GetToolData()->PropBuildingSettingsData.bEnableSurfaceSnapping = bEnableSurfaceSnapping;

			if(!bEnableSurfaceSnapping)
			{
				if(const auto IMBManinScreen = Cast<IMBMainScreenInterface>(ToolSettingsSubsystem->GetToolMainScreen().Get()))
				{
					if(const auto IMBBuildingManager = Cast<IMBBuildingManagerInterface>(IMBManinScreen->GetBuildingManager()))
					{
						IMBBuildingManager->ResetPropSnapRotation();
					}
				}	
			}
		}
	}
	if(	PropertyChangedEvent.GetPropertyName() == TEXT("bScaleRateCond")||
		PropertyChangedEvent.GetPropertyName() == TEXT("ScaleRate"))
	{
		if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
		{
			if(const auto IMBBuildingManager = Cast<IMBBuildingManagerInterface>(Cast<IMBMainScreenInterface>(ToolSettingsSubsystem->GetToolMainScreen())->GetBuildingManager()))
			{
				IMBBuildingManager->ApplyScaleRate();
			}
		}
	}
}


