// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Data/UTToolMenuCommands.h"

#define HelpLink "https://drive.google.com/file/d/1Hqwvq32vmGx-SFaC0t12otKMl7BWLURd/view"

class FUltimateLevelArtToolModule : public IModuleInterface
{
public:
	
	/* IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void InitToolMenuCommands();
	void SetupPluginToolbarEntry();
	TSharedRef<class SWidget> CreateToolMenu() const;
	void AddToolbarExtension(class FToolBarBuilder& ToolBarBuilder);

	FName MBToolTabID;
	FName ODToolTabID;
	FName MAToolTabID;
	FName ASToolTabID;

	TSharedPtr<class FUICommandList> ToolMenuCommands;

public:
	void ToggleModularBuildingWindow(); 
	void ToggleObjectDistributionWindow();
	void ToggleAutoSplineWindow();
	void ToggleMaterialAssignmentWindow();
	void LaunchHelp();
};
