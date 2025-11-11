// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBModularBuildingSettings.h"
#include "Editor.h"
#include "MBMBuildingSettingsObj.h"
#include "MBToolData.h"
#include "MBToolSubsystem.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/DetailsView.h"
#include "Components/TextBlock.h"
#include "Data/MBModularEnum.h"

void UMBModularBuildingSettings::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR

	if((BuildingSettingsObj = Cast<UMBMBuildingSettingsObj>(NewObject<UMBMBuildingSettingsObj>(this, TEXT("ModularBuildingSettingsObj")))))
	{
		BuildingSettingsObj->SetupObjectSettings();
		MBDetails->SetObject(BuildingSettingsObj);
	}

#endif
}

void UMBModularBuildingSettings::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!PlacementTypeBtn->OnClicked.IsBound())
	{
		PlacementTypeBtn->OnClicked.AddDynamic(this, &UMBModularBuildingSettings::PlacementTypeBtnPressed);
	}

	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		ToolSettingsSubsystem->OnPlacementTypeChanged.AddDynamic(this,&UMBModularBuildingSettings::OnPlacementTypeChanged);
	}
	
	PlacementTypeBtn->SetToolTipText(FText::FromString(TEXT("Change the placement type ( Shift )")));

	RefreshPlacementText();
}

void UMBModularBuildingSettings::NativeDestruct()
{
	if(PlacementTypeBtn){PlacementTypeBtn->OnClicked.RemoveAll(this);}
	
	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		ToolSettingsSubsystem->OnPlacementTypeChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UMBModularBuildingSettings::PlacementTypeBtnPressed()
{
	if(const auto ToolSettingsSubsystem  = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		ToolSettingsSubsystem->ChangePlacementType();
	}
}


void UMBModularBuildingSettings::OnPlacementTypeChanged()
{
	RefreshPlacementText();
}

void UMBModularBuildingSettings::RefreshPlacementText() const
{
	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.PlacementType == EPlacementType::Single)
		{
			PlacementTypeText->SetText(FText::FromName("Repetitive Placement"));

			FButtonStyle Style = PlacementTypeBtn->GetStyle();
			Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("0A75A4FF")));
			Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("0A75A4FF")));
			Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("0A75A4FF")));
			Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("0A75A4FF")));
			PlacementTypeBtn->SetStyle(Style);
		}
		else
		{
			PlacementTypeText->SetText(FText::FromName("Multiple Placement"));

			FButtonStyle Style = PlacementTypeBtn->GetStyle();
			Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("A98143FF")));
			Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("A98143FF")));
			Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("A98143FF")));
			Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("A98143FF")));
			PlacementTypeBtn->SetStyle(Style);
		}
	}
}
