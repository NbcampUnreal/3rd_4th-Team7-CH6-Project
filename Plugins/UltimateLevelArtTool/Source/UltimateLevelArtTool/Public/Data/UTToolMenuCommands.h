// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#pragma once

#include "Framework/Commands/Commands.h"

class FUTToolMenuCommands : public TCommands<FUTToolMenuCommands>
{
public:
	FUTToolMenuCommands() : TCommands<FUTToolMenuCommands>(
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

