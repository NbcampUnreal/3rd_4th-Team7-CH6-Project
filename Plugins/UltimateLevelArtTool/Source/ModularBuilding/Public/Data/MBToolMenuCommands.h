// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"

class FMBToolMenuCommands : public TCommands<FMBToolMenuCommands>
{
public:
	FMBToolMenuCommands() : TCommands<FMBToolMenuCommands>(
	TEXT("Leartes Studios"),
	FText::FromString(TEXT("Leartes Studios Commands")),
	NAME_None,
	TEXT("Leartes Studios")
	) {}

	virtual void RegisterCommands() override;
	
	TSharedPtr<FUICommandInfo> OpenModularBuildingTool;
	TSharedPtr<FUICommandInfo> OpenObjectDistributionTool;
	TSharedPtr<FUICommandInfo> OpenAutoSplineTool;
	TSharedPtr<FUICommandInfo> OpenMaterialAssignmentTool;
	TSharedPtr<FUICommandInfo> LaunchHelp;
	
	TArray<TSharedPtr<FUICommandInfo>> ToolCommands;
};

