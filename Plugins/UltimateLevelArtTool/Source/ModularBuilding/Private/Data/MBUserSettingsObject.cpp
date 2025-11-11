// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Data/MBUserSettingsObject.h"
#include "MBToolSubsystem.h"
#include "MBUserSettings.h"
#include "Editor.h"

void UMBUserSettingsObject::InitializeSettings()
{
	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSettingsSubsystem){return;}

	if(const auto ToolUserSettings = ToolSettingsSubsystem->GetToolUserSettings())
	{
		ObjectTracingDistance = ToolUserSettings->ObjectTracingDistance;
		FreePlacementDistance = ToolUserSettings->FreePlacementDistance;
		UsePreviewShader = ToolUserSettings->UsePreviewShader;
		PlaceableColor = ToolUserSettings->PlaceableColor;
		NotPlaceableColor = ToolUserSettings->NotPlaceableColor;
		ReplaceableColor = ToolUserSettings->ReplaceableColor;
		EnableDirectionDebugger = ToolUserSettings->EnableDirectionDebugger;
		DirectionDebugColor = ToolUserSettings->DirectionDebugColor;
		DebugThickness = ToolUserSettings->DebugThickness;
		AssetSelectionColor = ToolUserSettings->AssetSelectionColor;
		CollectionSelectionColor = ToolUserSettings->CollectionSelectionColor;
	}
}

void UMBUserSettingsObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSettingsSubsystem){return;}

	if(const auto ToolUserSettings = ToolSettingsSubsystem->GetToolUserSettings())
	{
		ToolUserSettings->ObjectTracingDistance = ObjectTracingDistance;
		ToolUserSettings->FreePlacementDistance = FreePlacementDistance;
		ToolUserSettings->UsePreviewShader = UsePreviewShader;
		ToolUserSettings->PlaceableColor = PlaceableColor;
		ToolUserSettings->NotPlaceableColor = NotPlaceableColor;
		ToolUserSettings->ReplaceableColor = ReplaceableColor;
		ToolUserSettings->EnableDirectionDebugger = EnableDirectionDebugger;
		ToolUserSettings->DirectionDebugColor = DirectionDebugColor;
		ToolUserSettings->DebugThickness = DebugThickness;
		ToolUserSettings->AssetSelectionColor = AssetSelectionColor;
		ToolUserSettings->CollectionSelectionColor = CollectionSelectionColor;
		ToolUserSettings->SaveConfig();
	}
	UObject::PostEditChangeProperty(PropertyChangedEvent);
}


