// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#if WITH_EDITOR

#include "ODDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Editor.h"
#include "ODPresetObject.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ODPresetData.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "ODToolSubsystem.h"
#include "Widgets/Input/STextComboBox.h"
#include "Components/SlateWrapperTypes.h"
#include "ODDistributionBase.h"
#include "ODToolStyle.h"


#define LOCTEXT_NAMESPACE "ObjectDistribution" 

#pragma region PresetDetails

void FODPresetDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& PresetCategory = DetailBuilder.EditCategory(("Presets"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));
	TArray<TWeakObjectPtr<UObject>> CustomizeObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizeObjects);

	if(CustomizeObjects.Num() != 1){return;}

	if(const auto PresetObject = Cast<UODPresetObject>(CustomizeObjects[0]))
	{
		PresetObject->OnPresetUpdated.BindRaw(this,&FODPresetDetailCustomization::OnPresetLoaded);

		PresetCategory.OnExpansionChanged(FOnBooleanValueChanged::CreateLambda([PresetObject](bool NewValue)
		{
			if(PresetObject)
			{
				PresetObject->PresetExpansionStateChanged(NewValue);
			}
		}));
	}

	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}


	const FName LastPreset = ToolSubsystem->GetLastSelectedPreset();
	auto Presets = ToolSubsystem->GetPresetNames();
	int32 SelectedPresetIndex = 0;
	
	for(int32 Index = 0; Index < Presets.Num(); ++Index)
	{
		PresetNames.Add(MakeShareable(new FString(Presets[Index])));

		if(!LastPreset.IsNone() && LastPreset.ToString().Equals(Presets[Index]))
		{
			SelectedPresetIndex = Index;
		}
	}
	
	const bool bIsInMixerMode = ToolSubsystem->bInMixerMode;

	if(!MixBtnStyle.IsValid())
	{
		MixBtnStyle =  MakeShareable(new FButtonStyle(FCoreStyle::Get().GetWidgetStyle< FButtonStyle >( "Button")));
	}
	
	//Mixer Btn Text
	FText MixerBtnLabel = FText();
	
	if(bIsInMixerMode)
	{
		MixerBtnLabel = FText(LOCTEXT("ButtonText","Close Mixer Mode"));
		MixBtnStyle->Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("1E5428FF")));
		MixBtnStyle->Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("1E5428FF")));
		MixBtnStyle->Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("1E5428FF")));
		MixBtnStyle->Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("1E5428FF")));
	}
	else
	{
		MixerBtnLabel = FText(LOCTEXT("ButtonText","Open Mixer Mode"));
		MixBtnStyle->Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("2F2F2FFF")));
		MixBtnStyle->Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("2F2F2FFF")));
		MixBtnStyle->Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("2F2F2FFF")));
		MixBtnStyle->Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("2F2F2FFF")));
	}
	
	PresetCategory.AddCustomRow(LOCTEXT("RowSearchName", "Presets"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Presets"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Center)
		//.FillWidth(1.0)
		.Padding(5,0,5,0)
		.AutoWidth()
		[
			SNew(SBox)
			.MinDesiredWidth(120)
			.HAlign(HAlign_Fill)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				[
					//Preset ComboBox
					SAssignNew(PresetComboBox,STextComboBox)
				   .Font(IDetailLayoutBuilder::GetDetailFont())
				   .OptionsSource(&PresetNames)
				   .InitiallySelectedItem(PresetNames[SelectedPresetIndex])
				   .OnSelectionChanged_Lambda([](const TSharedPtr<FString>& String, ESelectInfo::Type)
				   {
					   if(const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>())
					   {
						   ToolSubsystemLocal->SetActivePreset(*String.Get());
			
						   ToolSubsystemLocal->LoadActivePreset();
					   }
				   })
				]
				+SOverlay::Slot()
				[
					//Mixer ComboBox
					SAssignNew(MixerComboBox,SComboButton)
					.OnGetMenuContent(this,&FODPresetDetailCustomization::GetPresetMixerMenuContent)
					.ButtonContent()
					[
						SNew(STextBlock)
						.Text(FText::FromName(TEXT("Mix Presets")))
						.Font(IDetailLayoutBuilder::GetDetailFont())
						.ToolTipText(FText::FromName(TEXT("It allows you to use multiple presets at the same time by combining them.")))
					]
				]
			]
		]
		
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Center)
		.Padding(5,0,0,0)
		.AutoWidth()
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			[
				SAssignNew(OpenTheMixerBtn,SButton)
				.HAlign(HAlign_Fill)
				.Text(MixerBtnLabel)
				.ButtonStyle(MixBtnStyle.Get())
				.OnClicked_Lambda([&DetailBuilder]() -> FReply
				{
					if(const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>())
					{
						ToolSubsystemLocal->ToggleMixerMode();
						
						DetailBuilder.ForceRefreshDetails();
					}
					return FReply::Handled();
				})
			]
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Right)
		.Padding(15,0,0,0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(16)
			.HeightOverride(16)
			[
				//Stop Button
				SNew(SButton)
				.ButtonStyle( &FODToolStyle::GetCreatedToolSlateStyleSet()->GetWidgetStyle<FButtonStyle>(FName("ObjectDistribution.ODSimButtonStyle")))
				.ContentPadding(FMargin(0))
				.ToolTipText(LOCTEXT("StopSimulationToolTipText", "Open Settings Menu"))
				.OnClicked_Lambda([]() ->FReply
				{
					if(const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>())
					{
						ToolSubsystemLocal->SpawnSettingsMenu();
					}
					return FReply::Handled();
				})
				[
					SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.Settings"))
				]
			]
		]
	];
	
	if(ToolSubsystem->bInMixerMode)
	{
		PresetComboBox->SetVisibility(EVisibility::Hidden);
		MixerComboBox->SetVisibility(EVisibility::Visible);
	}
	else
	{
		MixerComboBox->SetVisibility(EVisibility::Hidden);
		PresetComboBox->SetVisibility(EVisibility::Visible);
	}

	CheckMixerBtnAvailability();
}

TSharedRef<IDetailCustomization> FODPresetDetailCustomization::MakeInstance()
{
	return MakeShareable(new FODPresetDetailCustomization);
}



TSharedRef<SWidget> FODPresetDetailCustomization::GetPresetMixerMenuContent()
{
	FMenuBuilder MenuBuilder(false, nullptr);

	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	TArray<FPresetMixerMapData>& MixerMap  = ToolSubsystem->GetPresetMixerMapData();
	
	if(MixerMap.Num() == 0){return SNullWidget::NullWidget;}
	
	for(FPresetMixerMapData& CurrentData : MixerMap)
	{
		MenuBuilder.AddMenuEntry(
			FText::FromName(CurrentData.PresetName),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
			FExecuteAction::CreateLambda([&CurrentData]()
			{
				const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
				if(!ToolSubsystem){return;}
				
				CurrentData.CheckState = !CurrentData.CheckState;
				ToolSubsystem->MixerPresetChecked(CurrentData);
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([&CurrentData]
			{
				return CurrentData.CheckState;
			})),
			NAME_None,
			EUserInterfaceActionType::Check
		);
	}
	return MenuBuilder.MakeWidget();
}

void FODPresetDetailCustomization::OnPresetLoaded()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
	
	PresetNames.Empty();
	
	const FName LastPreset = ToolSubsystem->GetLastSelectedPreset();
	auto Presets = ToolSubsystem->GetPresetNames();
	int32 SelectedPresetIndex = 0;
	
	for(int32 Index = 0; Index < Presets.Num(); ++Index)
	{
		PresetNames.Add(MakeShareable(new FString(Presets[Index])));

		if(!LastPreset.IsNone() && LastPreset.ToString().Equals(Presets[Index]))
		{
			SelectedPresetIndex = Index;
		}
	}

	PresetComboBox->SetSelectedItem(PresetNames[SelectedPresetIndex]);

	CheckMixerBtnAvailability();
}

void FODPresetDetailCustomization::CheckMixerBtnAvailability()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}

	if(ToolSubsystem->bInMixerMode)
	{
		OpenTheMixerBtn->SetToolTipText(FText(LOCTEXT("ButtonTooltip","Switch back to preset mode.")));
		OpenTheMixerBtn->SetEnabled(true);

	}
	else
	{
		if(PresetNames.Num() > 2)
		{
			OpenTheMixerBtn->SetEnabled(true);
			OpenTheMixerBtn->SetToolTipText(FText(LOCTEXT("ButtonTooltip","Switch to mixer mode and merge existing presets.")));
		}
		else
		{
			OpenTheMixerBtn->SetEnabled(false);
			OpenTheMixerBtn->SetToolTipText(FText(LOCTEXT("ButtonTooltip","Switch to mixer mode and merge existing presets.\nYou must have at least 2 presets to use this feature.")));
		}
	}
}

#pragma endregion PresetDetails

#pragma region DistributionDetails

void FODDistributionDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	/*IDetailCategoryBuilder& CreationCategory = */DetailBuilder.EditCategory(("Creation"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));
	/*IDetailCategoryBuilder& DistributionCategory = */DetailBuilder.EditCategory(("Distribution"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(1));
	
	/*TArray<TWeakObjectPtr<UObject>> CustomizeObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizeObjects);

	if(CustomizeObjects.Num() != 1){return;}
	
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}*/
	
/*#pragma region CreationButtons
	
	auto ODDistBaseObj = Cast<UODDistributionBase>(CustomizeObjects[0].Get());

	CreationCategory.AddCustomRow(LOCTEXT("RowSearchName","Creation"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Creation"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Create Distribution"))
		.ToolTipText(LOCTEXT("ButtonTooltip","Create objects and distribute them according to the specified Object Distribution Data."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnCreateDistributionPressed();
			}
			return FReply::Handled();
		})
	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Delete Distribution"))
		.ToolTipText(LOCTEXT("ButtonTooltip","Destroy the created objects."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnDeleteDistributionPressed();
			}
			return FReply::Handled();
		})
	]
	];
#pragma endregion CreationButtons*/

/*#pragma region DistributionButtons

	DistributionCategory.AddCustomRow(LOCTEXT("RowSearchName","Shuffle"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Object Operations"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Shuffle Distribution"))
		.ToolTipText(LOCTEXT("ButtonTooltip"," Shuffle the order of objects and redistribute them."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnShuffleDistributionPressed();
			}
			return FReply::Handled();
		})
	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(5,0,5,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Select Objects"))
		.ToolTipText(LOCTEXT("ButtonTooltip","Select all the created objects."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnSelectObjectsPressed();
			}
			return FReply::Handled();
		})
	]
	];
#pragma endregion DistributionButtons*/
	
/*#pragma region SimulationnButtons

	SimulationCategory.AddCustomRow(LOCTEXT("RowSearchName","Simulate"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Simulate"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SBox)
		.MinDesiredWidth(140)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText","Start Simulation"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip","Simulate the distributed objects."))
			.OnClicked_Lambda([ODDistBaseObj]() -> FReply
			{
				if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
				{
					ODDistBaseObj->OnStartSimulationPressed();
				}
				return FReply::Handled();
			})
		]

	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(5,0,0,0)
	.AutoWidth()
	[
		SNew(SBox)
		.MinDesiredWidth(140)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText","Stop Simulation"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip","Stop the simulation."))
			.OnClicked_Lambda([ODDistBaseObj]() -> FReply
			{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnStopSimulationPressed();
			}
			return FReply::Handled();
			})
		]
	]
	];

	SimulationCategory.AddCustomRow(LOCTEXT("RowSearchName","Simulate"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Simulate"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SBox)
		.MinDesiredWidth(140)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText","Pause Simulation"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip","Pause the simulation."))
			.OnClicked_Lambda([ODDistBaseObj]() -> FReply
			{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnPauseSimulationPressed();
			}
			return FReply::Handled();
			})
		]

	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(5,0,0,0)
	.AutoWidth()
	[
		SNew(SBox)
		.MinDesiredWidth(140)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText","Resume Simulation"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip","Resume the simulation."))
			.OnClicked_Lambda([ODDistBaseObj]() -> FReply
			{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnResumeSimulationPressed();
			}
			return FReply::Handled();
			})
		]
	]
	];

#pragma endregion SimulationnButtons*/

/*#pragma region SpawnCenterButtons
	
	SpawnCenterCategory.AddCustomRow(LOCTEXT("RowSearchName","Selection"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Selection"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,0,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Select Spawn Center"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip","Select the spawn center."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnSelectSpawnCenterPressed();
			}
			return FReply::Handled();
		})
	]
	];
	
	SpawnCenterCategory.AddCustomRow(LOCTEXT("RowSearchName","Move"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Move Spawn Center"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Move To World Center"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip","Move the spawn center to the world center."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnMoveSpawnCenterToWorldOriginPressed();
			}
			return FReply::Handled();
		})
	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(5,0,0,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Move To Camera"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip","Move the spawn center to the field of view."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->OnMoveSpawnCenterToCameraPressed();
			}
			return FReply::Handled();
		})
	]
	];
#pragma endregion SpawnCenterButtons*/

/*#pragma region SettingsButtons

	SettingsCategory.AddCustomRow(LOCTEXT("RowSearchName","Reset Tool"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Reset Tool Parameters"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(0,0,5,0)
	.AutoWidth()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Reset Tool"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip","Reset all tool parameters except presets to defaults."))
		.OnClicked_Lambda([ODDistBaseObj]() -> FReply
		{
			if(IsValid(ODDistBaseObj) && ODDistBaseObj->IsA(UODDistributionBase::StaticClass()))
			{
				ODDistBaseObj->ResetParametersPressed();
			}
			return FReply::Handled();
		})
	]];
#pragma endregion SettingsButtons*/
	
}

TSharedRef<IDetailCustomization> FODDistributionDetailCustomization::MakeInstance()
{
	return MakeShareable(new FODDistributionDetailCustomization);
}

#pragma endregion DistributionDetails


#pragma region PresetDetails

void FODPaletteObjectDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	/*IDetailCategoryBuilder& ObjectDataCategory = */DetailBuilder.EditCategory(("Object Data"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));
}

TSharedRef<IDetailCustomization> FODPaletteObjectDetails::MakeInstance()
{
	return MakeShareable(new FODPaletteObjectDetails);
}

void FODSettingsObjectDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	/*IDetailCategoryBuilder& SimulationCategory = */DetailBuilder.EditCategory(("Simulation"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));
	/*IDetailCategoryBuilder& VisualizationCategory = */DetailBuilder.EditCategory(("Visualization"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(1));
}

TSharedRef<IDetailCustomization> FODSettingsObjectDetails::MakeInstance()
{
	return MakeShareable(new FODSettingsObjectDetails);
}

#undef LOCTEXT_NAMESPACE

#endif


