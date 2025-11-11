// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBSettingMenu.h"
#include "Editor.h"
#include "Components/ContentWidget.h"
#include "MBToolAssetData.h"
#include "Data/MBToolData.h"
#include "MBToolSubsystem.h"


void UMBSettingMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//RegenerateTheSettings();
	
}

void UMBSettingMenu::NativeDestruct()
{



	
	Super::NativeDestruct();
}

void UMBSettingMenu::RegenerateTheSettings() const
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();

	if(!ToolSettingsSubsystem || !EditorWorld){return;}

	if(ContentBox){ContentBox->ClearChildren();}
	
	if(ToolSettingsSubsystem->WorkingMode == EMBWorkingMode::Placement || ToolSettingsSubsystem->WorkingMode == EMBWorkingMode::None)
	{
		const auto AssetCategory = ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory;
		
		if(AssetCategory == EBuildingCategory::Modular)
		{
			
			const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ModularBuildingSettingsPath);
			if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
			{
				if (const auto CreatedSubSettingsWidget = Cast<UUserWidget>(CreateWidget(EditorWorld, ClassRef)))
				{
					if(CreatedSubSettingsWidget)
					{
						ContentBox->AddChild(CreatedSubSettingsWidget);
					}
				}
			}
		}
		else if(AssetCategory == EBuildingCategory::Prop)
		{
			const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::PropBuildingSettingsPath);
			if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
			{
				if (const auto CreatedSubSettingsWidget = Cast<UUserWidget>(CreateWidget(EditorWorld, ClassRef)))
				{
					ContentBox->AddChild(CreatedSubSettingsWidget);
				}
			}
		}
		
	}
	else if(ToolSettingsSubsystem->WorkingMode == EMBWorkingMode::ModActorAction)
	{
		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ModularActorActionsPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (const auto CreatedSubSettingsWidget = Cast<UUserWidget>(CreateWidget(EditorWorld, ClassRef)))
			{
				if(CreatedSubSettingsWidget)
				{
					ContentBox->AddChild(CreatedSubSettingsWidget);
				}
			}
		}
	}
	else if(ToolSettingsSubsystem->WorkingMode == EMBWorkingMode::PropActorAction)
	{
		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::PropActorActionsPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (const auto CreatedSubSettingsWidget = Cast<UUserWidget>(CreateWidget(EditorWorld, ClassRef)))
			{
				if(CreatedSubSettingsWidget)
				{
					ContentBox->AddChild(CreatedSubSettingsWidget);
				}
			}
		}
	}	
}

