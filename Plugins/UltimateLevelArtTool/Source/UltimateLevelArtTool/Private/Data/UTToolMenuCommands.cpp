// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Data/UTToolMenuCommands.h"
#include "Style/UTToolStyle.h"
#include "Framework/Commands/Commands.h"

#define LOCTEXT_NAMESPACE "FUltimateLevelArtToolModule"

void FUTToolMenuCommands::RegisterCommands()
{
	const FText MBLabel = FText::FromString(TEXT("Modular Building"));
	const FText MBDesc = FText::FromString(TEXT("Quickly build modular structures."));
	
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		OpenModularBuildingTool,
		"ModularBuildingTool",
		MBLabel,
		MBDesc,
		FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.ModularBuildingIcon"),
		EUserInterfaceActionType::Button,
		FInputChord());

	/*******************************************************************************/
	
	const FText ODLabel = FText::FromString(TEXT("Object Distribution"));
	const FText ODDesc = FText::FromString(TEXT("Quickly create and simulate static meshes."));
	
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		OpenObjectDistributionTool,
		"ObjectDistributionTool",
		ODLabel,
		ODDesc,
		FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.DistributionIcon"),
		EUserInterfaceActionType::Button,
		FInputChord());
		
	/*******************************************************************************/
	
	const FText ASLabel = FText::FromString(TEXT("Auto Spline"));
	const FText ASDesc = FText::FromString(TEXT("Quickly create and distribute static meshes, actors or components on custom splines."));
	
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		OpenAutoSplineTool,
		"AutoSplineTool",
		ASLabel,
		ASDesc,
		FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.AutoSplineIcon"),
		EUserInterfaceActionType::Button,
		FInputChord());

	/*******************************************************************************/
	
	const FText MALabel = FText::FromString(TEXT("Material Assignment"));
	const FText MADesc = FText::FromString(TEXT("Quickly assign materials to static meshes collectively."));
	
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		OpenMaterialAssignmentTool,
		"MaterialAssignmentTool",
		MALabel,
		MADesc,
		FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.MaterialAssignmentIcon"),
		EUserInterfaceActionType::Button,
		FInputChord());
	

	/*******************************************************************************/
	
	const FText HelpLabel = FText::FromString(TEXT("Help..."));
	const FText HelpDesc = FText::FromString(TEXT("Go to Documentation"));
	
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		LaunchHelp,
		"LaunchHelp",
		HelpLabel,
		HelpDesc,
		FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.HelpIcon"),
		EUserInterfaceActionType::Button,
		FInputChord());
}

#undef LOCTEXT_NAMESPACE
