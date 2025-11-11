// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "AutoSplineEditor.h"
#include "ASDetailCustomization.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FAutoSplineEditorModule"

void FAutoSplineEditorModule::StartupModule()
{
	
	RegisterCustomDetailClasses();
	
}

void FAutoSplineEditorModule::ShutdownModule()
{
	UnregisterCustomDetailClasses();
	
}

void FAutoSplineEditorModule::RegisterCustomDetailClasses() const
{
	auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("ASDeformMeshSpline",FOnGetDetailCustomizationInstance::CreateStatic(&FASDeformMeshDetailCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ASCustomClassSpline",FOnGetDetailCustomizationInstance::CreateStatic(&FASCustomClassDetailCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ASStaticMeshDistributionSpline",FOnGetDetailCustomizationInstance::CreateStatic(&FASMeshDistributionDetailCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}



void FAutoSplineEditorModule::UnregisterCustomDetailClasses() const
{
	auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("ASDeformMeshSpline");
	PropertyModule.UnregisterCustomClassLayout("ASCustomClassSpline");
	PropertyModule.UnregisterCustomClassLayout("ASStaticMeshDistributionSpline");
	PropertyModule.NotifyCustomizationModuleChanged();
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAutoSplineEditorModule, AutoSplineEditor)