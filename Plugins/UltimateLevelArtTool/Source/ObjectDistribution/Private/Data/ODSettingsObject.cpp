// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODSettingsObject.h"

#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

void UODSettingsObject::SetupSettingsObject()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		bSimulatePhysics = ToolSubsystem->GetODToolSettings()->bSimulatePhysics;
		bDisableSimAfterFinish = ToolSubsystem->GetODToolSettings()->bDisableSimAfterFinish;
		bDrawSpawnBounds = ToolSubsystem->GetODToolSettings()->bDrawSpawnBounds;
		BoundsColor = ToolSubsystem->GetODToolSettings()->BoundsColor;
	}
}

void UODSettingsObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		ToolSubsystem->GetODToolSettings()->bSimulatePhysics = bSimulatePhysics;
		ToolSubsystem->GetODToolSettings()->bDisableSimAfterFinish = bDisableSimAfterFinish;
		ToolSubsystem->GetODToolSettings()->bDrawSpawnBounds = bDrawSpawnBounds;
		ToolSubsystem->GetODToolSettings()->BoundsColor = BoundsColor;

		ToolSubsystem->SettingsMenuParamChanged(PropertyChangedEvent.GetPropertyName());
		
		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			ToolSubsystem->GetODToolSettings()->SaveConfig();
		}
	}
}
