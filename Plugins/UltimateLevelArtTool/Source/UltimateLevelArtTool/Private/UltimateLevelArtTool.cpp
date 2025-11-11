// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UltimateLevelArtTool.h"
#include "Editor.h"
#include "EditorUtilitySubsystem.h" 
#include "EditorUtilityWidgetBlueprint.h"
#include "LevelEditor.h"
#include "UTToolAssetData.h"
#include "Editor/UnrealEdEngine.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Style/UTToolStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "FUltimateLevelArtToolModule"

void FUltimateLevelArtToolModule::StartupModule()
{
	FUTToolStyle::InitializeToolStyle();
	
	FUTToolMenuCommands::Register();
	InitToolMenuCommands();
	
	SetupPluginToolbarEntry();
}

void FUltimateLevelArtToolModule::InitToolMenuCommands()
{
	ToolMenuCommands = MakeShareable(new FUICommandList());
	
	ToolMenuCommands->MapAction(FUTToolMenuCommands::Get().OpenModularBuildingTool,FExecuteAction::CreateRaw(this,&FUltimateLevelArtToolModule::ToggleModularBuildingWindow));
	ToolMenuCommands->MapAction(FUTToolMenuCommands::Get().OpenObjectDistributionTool,FExecuteAction::CreateRaw(this,&FUltimateLevelArtToolModule::ToggleObjectDistributionWindow));
	ToolMenuCommands->MapAction(FUTToolMenuCommands::Get().OpenAutoSplineTool,FExecuteAction::CreateRaw(this,&FUltimateLevelArtToolModule::ToggleAutoSplineWindow));
	ToolMenuCommands->MapAction(FUTToolMenuCommands::Get().OpenMaterialAssignmentTool,FExecuteAction::CreateRaw(this,&FUltimateLevelArtToolModule::ToggleMaterialAssignmentWindow));
	ToolMenuCommands->MapAction(FUTToolMenuCommands::Get().LaunchHelp,FExecuteAction::CreateRaw(this,&FUltimateLevelArtToolModule::LaunchHelp));
}


void FUltimateLevelArtToolModule::SetupPluginToolbarEntry()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<FExtender> Extenders = LevelEditorModule.GetToolBarExtensibilityManager()->GetAllExtenders();
	
	const TSharedPtr<FExtender> ODToolExtender = MakeShareable(new FExtender);
	ODToolExtender->AddToolBarExtension("Play", EExtensionHook::After, NULL, FToolBarExtensionDelegate::CreateRaw(this, &FUltimateLevelArtToolModule::AddToolbarExtension));
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ODToolExtender);
}

void FUltimateLevelArtToolModule::AddToolbarExtension(FToolBarBuilder& ToolBarBuilder)
{
	const auto UnitsWidgetLambda = [this]() -> TSharedRef<SWidget> {
		return
		SNew(SComboButton)
		.OnGetMenuContent(FOnGetContent::CreateRaw(this, &FUltimateLevelArtToolModule::CreateToolMenu))
		.ComboButtonStyle(FAppStyle::Get(),"SimpleComboButton")
		.ButtonContent()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.Padding(0,0,2,0)
			[
				SNew(SBox)
				.WidthOverride(24)
				.HeightOverride(24)
				[
					SNew(SImage)
					.Image(FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.LeartesStudiosToolIcon").GetIcon())
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.Padding(0)
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.TextStyle(FAppStyle::Get(), "Menu.Label")
				.Text(LOCTEXT("UltimateLevelArtTool", "Ultimate Level Art Tool"))
				]
			]
		];
	};
	
	ToolBarBuilder.AddWidget(UnitsWidgetLambda());

}

TSharedRef<SWidget> FUltimateLevelArtToolModule::CreateToolMenu() const
{
	FMenuBuilder MenuBuilder(true, ToolMenuCommands);
	MenuBuilder.BeginSection("Tools");
	MenuBuilder.AddMenuEntry(FUTToolMenuCommands::Get().OpenModularBuildingTool.ToSharedRef());
	MenuBuilder.AddMenuEntry(FUTToolMenuCommands::Get().OpenObjectDistributionTool.ToSharedRef());
	MenuBuilder.AddMenuEntry(FUTToolMenuCommands::Get().OpenAutoSplineTool.ToSharedRef());
	MenuBuilder.AddMenuEntry(FUTToolMenuCommands::Get().OpenMaterialAssignmentTool.ToSharedRef());
	MenuBuilder.AddSeparator();
	MenuBuilder.AddMenuEntry(FUTToolMenuCommands::Get().LaunchHelp.ToSharedRef());
	MenuBuilder.EndSection();
	//MenuBuilder.AddSubMenu()
	return MenuBuilder.MakeWidget();
}

void FUltimateLevelArtToolModule::ToggleModularBuildingWindow()
{
	if (!GEditor){return;}

	if(	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
	{
		if(EditorUtilitySubsystem->DoesTabExist(MBToolTabID))
		{
			EditorUtilitySubsystem->CloseTabByID(MBToolTabID);
		}
		else
		{
			if(UObject* ToolWindowObject = MBToolAssetData::MBToolWindowPath.TryLoad())
			{
				if(const auto ToolWindowBlueprint = Cast<UEditorUtilityWidgetBlueprint>(ToolWindowObject))
				{
					EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(ToolWindowBlueprint,MBToolTabID);

					//Set Tab Icon
					const FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
					const TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
					if (const TSharedPtr<SDockTab> FoundTab = LevelEditorTabManager->FindExistingLiveTab(MBToolTabID))
					{
						FoundTab->SetTabIcon(FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.ModularBuildingIcon").GetIcon());
					}
				}
			}
		}
	}
}

void FUltimateLevelArtToolModule::ToggleObjectDistributionWindow()
{
	if (!GEditor){return;}

	if(	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
	{
		if(EditorUtilitySubsystem->DoesTabExist(ODToolTabID))
		{
			EditorUtilitySubsystem->CloseTabByID(ODToolTabID);
		}
		else
		{
			if(UObject* ToolWindowObject = MBToolAssetData::ODToolWindowPath.TryLoad())
			{
				if(const auto ToolWindowBlueprint = Cast<UEditorUtilityWidgetBlueprint>(ToolWindowObject))
				{
					EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(ToolWindowBlueprint,ODToolTabID);
					//Set Tab Icon
					const FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
					const TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
					if (const TSharedPtr<SDockTab> FoundTab = LevelEditorTabManager->FindExistingLiveTab(ODToolTabID))
					{
						FoundTab->SetTabIcon(FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.DistributionIcon").GetIcon());
					}
				}
			}
		}
	}
}


void FUltimateLevelArtToolModule::ToggleAutoSplineWindow()
{
	if (!GEditor){return;}

	if(	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
	{
		if(EditorUtilitySubsystem->DoesTabExist(ASToolTabID))
		{
			EditorUtilitySubsystem->CloseTabByID(ASToolTabID);
		}
		else
		{
			if(UObject* ToolWindowObject = MBToolAssetData::ASToolWindowPath.TryLoad())
			{
				if(const auto ToolWindowBlueprint = Cast<UEditorUtilityWidgetBlueprint>(ToolWindowObject))
				{
					EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(ToolWindowBlueprint,ASToolTabID);

					//Fix Window Size
					const FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
					const TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
					if (const TSharedPtr<SDockTab> FoundTab = LevelEditorTabManager->FindExistingLiveTab(ASToolTabID))
					{
						FoundTab->SetTabIcon(FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.AutoSplineIcon").GetIcon());
						FWindowSizeLimits SizeLimits;
						SizeLimits.SetMaxHeight(537.0f);
						SizeLimits.SetMaxWidth(442.0f);

						if(FoundTab->GetParentWindow().IsValid())
						{
							FoundTab->GetParentWindow()->SetSizeLimits(SizeLimits);
							FoundTab->GetParentWindow()->Resize(FVector2D(442.0f,537.0f));
						}
					}
				}
			}
		}

	}


}

void FUltimateLevelArtToolModule::ToggleMaterialAssignmentWindow()
{
	if (!GEditor){return;}

	if(	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
	{
		if(EditorUtilitySubsystem->DoesTabExist(MAToolTabID))
		{
			EditorUtilitySubsystem->CloseTabByID(MAToolTabID);
		}
		else
		{
			if(UObject* ToolWindowObject = MBToolAssetData::MAToolWindowPath.TryLoad())
			{
				if(const auto ToolWindowBlueprint = Cast<UEditorUtilityWidgetBlueprint>(ToolWindowObject))
				{
					EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(ToolWindowBlueprint,MAToolTabID);

					//Fix Window Size
					const FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
					const TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
					if (const TSharedPtr<SDockTab> FoundTab = LevelEditorTabManager->FindExistingLiveTab(MAToolTabID))
					{
						FoundTab->SetTabIcon(FSlateIcon(FUTToolStyle::GetToolStyleName(),"LevelEditor.MaterialAssignmentIcon").GetIcon());

						FWindowSizeLimits SizeLimits;
						SizeLimits.SetMaxHeight(800.0f);
						SizeLimits.SetMaxWidth(820.0f);

						if(FoundTab->GetParentWindow().IsValid())
						{
							//FoundTab->GetParentWindow()->SetSizeLimits(SizeLimits);
							FoundTab->GetParentWindow()->Resize(FVector2D(820.0f,500.0f));
						}
					}
				}
			}
		}
	}
}


void FUltimateLevelArtToolModule::LaunchHelp()
{
	FPlatformProcess::LaunchURL(TEXT(HelpLink),nullptr,nullptr);
}

void FUltimateLevelArtToolModule::ShutdownModule()
{
	FUTToolStyle::ShutDownStyle();
	
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltimateLevelArtToolModule, UltimateLevelArtTool)