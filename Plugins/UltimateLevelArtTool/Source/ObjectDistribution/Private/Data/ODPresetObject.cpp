// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODPresetObject.h"
#include "Editor.h"
#include "ODToolSubsystem.h"

void UODPresetObject::SetupPresets()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
		
	ToolSubsystem->UpdateMixerCheckListAndResetAllCheckStatus();

	OnPresetUpdated.ExecuteIfBound();
}

void UODPresetObject::PresetExpansionStateChanged(bool& InIsItOpen)
{
	OnPresetCategoryHidden.ExecuteIfBound(InIsItOpen);
}

void UODPresetObject::BeginDestroy()
{
	UObject::BeginDestroy();
	OnPresetUpdated.Unbind();
}
