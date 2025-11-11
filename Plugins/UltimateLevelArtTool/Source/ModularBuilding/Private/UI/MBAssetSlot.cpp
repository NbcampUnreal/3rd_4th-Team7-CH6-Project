// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBAssetSlot.h"
#include "Editor.h"
#include "MBAssetToolTip.h"
#include "MBToolAssetData.h"
#include "MBUserSettings.h"
#include "ObjectTools.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MBToolSubsystem.h"
#include "Engine/StaticMesh.h"
#include "Interfaces/MBMainScreenInterface.h"
#include "Libraries/MBAssetFunctions.h"
#include "Modules/ModuleManager.h"


void UMBAssetSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	bSelectionState = false;
}

void UMBAssetSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (!AssetSelectionBtn->OnClicked.IsBound())
	{
		AssetSelectionBtn->OnClicked.AddDynamic(this, &UMBAssetSlot::AssetSelectionBtnPressed);
	}
}

void UMBAssetSlot::NativeDestruct()
{
	if(AssetSelectionBtn){AssetSelectionBtn->OnClicked.RemoveAll(this);}

	
	Super::NativeDestruct();
}

void UMBAssetSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if(!GEditor){return;}
	const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::AssetTooltipPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto AssetToolTipMenu = Cast<UMBAssetToolTip>(CreateWidget(GEditor->GetEditorWorldContext().World(), ClassRef)))
		{
			const auto AssetData = UMBAssetFunctions::GetAssetDataFromPath(AssetPath.ToString());
			AssetToolTipMenu->SetTooltipData(AssetData);
			AssetSelectionBtn->SetToolTip(AssetToolTipMenu);
		}
	}
}

void UMBAssetSlot::AssetSelectionBtnPressed()
{
	if(!GEditor){return;}
	if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{

		if(const auto ToolMainScreen = ToolSubsystem->GetToolMainScreen().Get())
		{
			Cast<IMBMainScreenInterface>(ToolMainScreen)->CreateAssetClicked(MeshNameText->GetText().ToString());
		}
		
		if(!ToolSubsystem->LastCreatedAsset.IsEmpty())
		{
			SetSelectionState(true);
		}
	}
}

void UMBAssetSlot::SetSelectionState(const bool InSelectionState)
{
	if(InSelectionState)
	{
		if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
		{
			if(const auto UserSettingsData = ToolSettingsSubsystem->GetToolUserSettings())
			{
				FButtonStyle Style = AssetSelectionBtn->GetStyle();
				Style.Normal.TintColor = UserSettingsData->AssetSelectionColor;
				Style.Pressed.TintColor = UserSettingsData->AssetSelectionColor;
				Style.Disabled.TintColor = UserSettingsData->AssetSelectionColor;
				Style.Hovered.TintColor = UserSettingsData->AssetSelectionColor;
				AssetSelectionBtn->SetStyle(Style);
				bSelectionState = true;
			}
		}
	}
	else
	{
		FButtonStyle Style = AssetSelectionBtn->GetStyle();
		Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("383838FF")));
		Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("383838FF")));
		Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("383838FF")));
		Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("595959FF")));
		AssetSelectionBtn->SetStyle(Style);
		bSelectionState = false;
	}
}

void UMBAssetSlot::SetSlotParams(const UStaticMesh* StaticMeshAsset)
{
	const auto AssetData = UMBAssetFunctions::GetAssetDataFromObject(StaticMeshAsset);
	AssetPath = AssetData.GetSoftObjectPath();
	if(const auto GeneratedAsset = UMBAssetFunctions::GenerateThumbnailForAsset(AssetData,EMBAssetThumbnailFormant::PNG))
	{
		MeshImage->SetBrushFromTexture(GeneratedAsset);
	}
	MeshNameText->SetText(FText::FromName(AssetData.AssetName));
}


FReply UMBAssetSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if(!GEditor){Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);}
		
		if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
		{
			if(ToolSettingsSubsystem->bIsCtrlPressed)
			{
				ToolSettingsSubsystem->RemoveAssetByName(MeshNameText->GetText().ToString());
			}
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}