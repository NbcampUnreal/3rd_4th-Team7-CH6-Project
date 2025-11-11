// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "ObjectDistribution.h"
#include "ODDetailCustomization.h"
#include "ODSimulationMode.h"
#include "ODToolStyle.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FObjectDistributionModule"

void FObjectDistributionModule::StartupModule()
{
	FODToolStyle::InitializeToolStyle();
	RegisterCustomDetailClasses();
	RegisterCustomEDMode();
}

void FObjectDistributionModule::ShutdownModule()
{
	FODToolStyle::ShutDownStyle();
	UnregisterCustomDetailClasses();

	//FEditorModeRegistry::Get().UnregisterMode(FODSimulationMode::OD_SimulationModeID);
}

void FObjectDistributionModule::RegisterCustomEDMode()
{
	/*// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FODSimulationMode>(
		FODSimulationMode::OD_SimulationModeID, 
		LOCTEXT("PhysicalLayoutEdModeName", "Physical Layout Mode"),
		FSlateIcon(FODToolStyle::GetToolStyleName(), "ObjectDistribution", "ObjectDistribution.ODPlayButtonIcon"),
		true);*/
}

void FObjectDistributionModule::RegisterCustomDetailClasses()
{
	auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("ODPresetObject",FOnGetDetailCustomizationInstance::CreateStatic(&FODPresetDetailCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ODDistributionBase",FOnGetDetailCustomizationInstance::CreateStatic(&FODDistributionDetailCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ODPaletteObjectDetails",FOnGetDetailCustomizationInstance::CreateStatic(&FODPaletteObjectDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ODSettingsObjectDetails",FOnGetDetailCustomizationInstance::CreateStatic(&FODSettingsObjectDetails::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FObjectDistributionModule::UnregisterCustomDetailClasses()
{
	auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("ODPresetObject");
	PropertyModule.UnregisterCustomClassLayout("ODDistributionBase");
	PropertyModule.UnregisterCustomClassLayout("ODPaletteObjectDetails");
	PropertyModule.UnregisterCustomClassLayout("ODSettingsObjectDetails");
	PropertyModule.NotifyCustomizationModuleChanged();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FObjectDistributionModule, ObjectDistribution)