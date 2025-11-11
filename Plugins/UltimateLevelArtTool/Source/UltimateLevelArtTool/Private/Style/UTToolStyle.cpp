// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Style/UTToolStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"


FName FUTToolStyle::ToolStyleName = FName("UTToolStyle");
TSharedPtr<FSlateStyleSet> FUTToolStyle::CreatedToolSlateStyleSet = nullptr;

void FUTToolStyle::InitializeToolStyle()
{	
	if(!CreatedToolSlateStyleSet.IsValid())
	{
		CreatedToolSlateStyleSet = CreateToolSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedToolSlateStyleSet);
	}
}

TSharedRef<FSlateStyleSet> FUTToolStyle::CreateToolSlateStyleSet()
{	
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(ToolStyleName));

	const FString IconDirectory = 
	IPluginManager::Get().FindPlugin(TEXT("UltimateLevelArtTool"))->GetBaseDir() /"Resources";
	
	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon32x32 (32.f,32.f);
	
	CustomStyleSet->Set("LevelEditor.DistributionIcon",
	new FSlateImageBrush(IconDirectory/"Object_Distribution.png",Icon32x32));

	CustomStyleSet->Set("LevelEditor.ModularBuildingIcon",
	new FSlateImageBrush(IconDirectory/"Modular_Building.png",Icon32x32));

	CustomStyleSet->Set("LevelEditor.MaterialAssignmentIcon",
	new FSlateImageBrush(IconDirectory/"Material_Assignment.png",Icon32x32));

	CustomStyleSet->Set("LevelEditor.AutoSplineIcon",
	new FSlateImageBrush(IconDirectory/"Auto_Spline.png",Icon32x32));
	
	CustomStyleSet->Set("LevelEditor.LeartesStudiosToolIcon",
    new FSlateImageBrush(IconDirectory/"ToolBar.png",Icon32x32));
	
	CustomStyleSet->Set("LevelEditor.HelpIcon",
	new FSlateImageBrush(IconDirectory/"Help.png",Icon32x32));
	
	return CustomStyleSet;
}

void FUTToolStyle::ShutDownStyle()
{
	if(CreatedToolSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedToolSlateStyleSet);
		CreatedToolSlateStyleSet.Reset();
	}
}
