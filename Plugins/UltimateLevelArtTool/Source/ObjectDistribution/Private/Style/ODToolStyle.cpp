// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODToolStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"


FName FODToolStyle::ToolStyleName = FName("ODToolStyle");
TSharedPtr<FSlateStyleSet> FODToolStyle::CreatedToolSlateStyleSet = nullptr;

void FODToolStyle::InitializeToolStyle()
{	
	if(!CreatedToolSlateStyleSet.IsValid())
	{
		CreatedToolSlateStyleSet = CreateToolSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedToolSlateStyleSet);
	}
}

TSharedRef<FSlateStyleSet> FODToolStyle::CreateToolSlateStyleSet()
{	
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(ToolStyleName));

	const FString IconDirectory = 
	IPluginManager::Get().FindPlugin(TEXT("UltimateLevelArtTool"))->GetBaseDir() /"Resources";
	
	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon24x24 (24.0f,24.0f);
	
	CustomStyleSet->Set("ObjectDistribution.ODPlayButtonIcon",
	new FSlateImageBrush(IconDirectory/"OD_PlayButton.png",Icon24x24));

	CustomStyleSet->Set("ObjectDistribution.ODStopButtonIcon",
	new FSlateImageBrush(IconDirectory/"OD_StopButton.png",Icon24x24));

	CustomStyleSet->Set("ObjectDistribution.ODPauseButtonIcon",
	new FSlateImageBrush(IconDirectory/"OD_PauseButton.png",Icon24x24));

	CustomStyleSet->Set("ObjectDistribution.ODAdvanceFrameButtonIcon",
	new FSlateImageBrush(IconDirectory/"OD_AdvanceFrameButton.png",Icon24x24));
	
	const FButtonStyle SimButtonStyle = FButtonStyle()
	.SetNormal(FSlateNoResource())
	.SetHovered(FSlateNoResource())
	.SetPressed(FSlateNoResource())
	.SetNormalPadding(FMargin(0,0,0,0))
	.SetPressedPadding(FMargin(1,1,1,1));

	CustomStyleSet->Set("ObjectDistribution.ODSimButtonStyle",SimButtonStyle);

	return CustomStyleSet;
}

void FODToolStyle::ShutDownStyle()
{
	if(CreatedToolSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedToolSlateStyleSet);
		CreatedToolSlateStyleSet.Reset();
	}
}
