// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODObjectSlot.h"
#include "Editor.h"
#include "ObjectTools.h"
#include "ODAssetFunctions.h"
#include "ODPaletteItemToolTip.h"
#include "ODToolAssetData.h"
#include "ODToolSubsystem.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Layout/WidgetPath.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SSlider.h"

#define LOCTEXT_NAMESPACE "PaletteObjectSlot"

UODObjectSlot::UODObjectSlot(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	bSelectionState = false;
	ActivateStatus = true;
	SlotIndex = -1;
	PresetName = FName();
}

void UODObjectSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	bSelectionState = false;
}


void UODObjectSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!SlotSelectBtn->OnClicked.IsBound())
	{
		SlotSelectBtn->OnClicked.AddDynamic(this, &UODObjectSlot::SlotSelectBtnPressed);
	}
	if(!ActivateCheckBox->OnCheckStateChanged.IsBound())
	{
		ActivateCheckBox->OnCheckStateChanged.AddDynamic(this,&UODObjectSlot::OnActivateCheckBoxChanged);
	}
}

void UODObjectSlot::SlotSelectBtnPressed()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastClickTime <= 0.35f)
	{
		if(CurrentTime - LastDoubleClickTime > 1.5f)
		{
			const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
			if(!ToolSubsystem){return;}

			ToolSubsystem->OpenPaletteItemOnSMEditor(SlotIndex);

			LastDoubleClickTime = GetWorld()->GetTimeSeconds();
		}
	}
	LastClickTime = CurrentTime;
	
	OnObjectSlotClickedSignature.ExecuteIfBound(bSelectionState,SlotIndex);
}

void UODObjectSlot::SetSlotParams(const FAssetData& AssetData,const bool InActivateStatus, const int32 InSlotIndex,const int32& InSpawnCount,FName InOwnerPreset,const FLinearColor InPresetColor)
{
	SlotIndex = InSlotIndex;
	ActivateStatus = InActivateStatus;
	PresetName = InOwnerPreset;
	CreateSlotThumbnail(AssetData);
	SetSpawnCount(InSpawnCount);

	if(IsValid(PresetBorder)){PresetBorder->SetBrushColor(InPresetColor);}
	
	if(IsValid(SlotImage)){SlotImage->SetColorAndOpacity(ActivateStatus ? FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("FFFFFFFF"))) : FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("797979FF"))));}

	if(IsValid(ActivateCheckBox)){ActivateCheckBox->SetCheckedState(ActivateStatus ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);}
	
	SetSelectionState(false);
}

void UODObjectSlot::CreateSlotThumbnail(const FAssetData& AssetData) const
{
	if(const auto GeneratedTexture = UODAssetFunctions::GenerateThumbnailForSM(AssetData))
	{
		SlotImage->SetBrushFromTexture(GeneratedTexture);
	}
	SlotSelectBtn->SetToolTipText(FText::FromName(AssetData.AssetName));
}

void UODObjectSlot::SetSpawnCount(const int32& InSpawnCount) const
{
	SpawnCountText->SetText(FText::FromString(FString::FromInt(InSpawnCount)));
}

void UODObjectSlot::ChangeActivateStatus(const bool& InNewStatus)
{
	ActivateStatus = InNewStatus;
	
	if(IsValid(SlotImage)){SlotImage->SetColorAndOpacity(ActivateStatus ? FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("FFFFFFFF"))) : FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("6E6E6EFF"))));}

	if(IsValid(ActivateCheckBox)){ActivateCheckBox->SetCheckedState(InNewStatus ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);}
	
	SetSelectionState(bSelectionState);
}


void UODObjectSlot::SetSelectionState(const bool InSelectionState)
{
	if(ActivateStatus)
	{
		if(InSelectionState)
		{
			FButtonStyle Style = SlotSelectBtn->GetStyle();
			Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("00C110FF")));
			Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("00EE16FF")));
			Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("005F04FF")));
			Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("8CEE85FF")));
			SlotSelectBtn->SetStyle(Style);
			bSelectionState = true;
		}
		else
		{
			FButtonStyle Style = SlotSelectBtn->GetStyle();
			Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("BBBBBBFF")));
			Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("595959FF")));
			Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("2F2F2FFF")));
			Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("BBBBBBFF")));
			SlotSelectBtn->SetStyle(Style);
			bSelectionState = false;
		}
	}
	else
	{
		if(InSelectionState)
		{
			FButtonStyle Style = SlotSelectBtn->GetStyle();
			Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("8CEE85FF")));
			Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("00EE16FF")));
			Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("005F04FF")));
			Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("8CEE85FF")));
			SlotSelectBtn->SetStyle(Style);
			bSelectionState = true;
		}
		else
		{
			FButtonStyle Style = SlotSelectBtn->GetStyle();
			Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("595959FF")));
			Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("595959FF")));
			Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("2F2F2FFF")));
			Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("BBBBBBFF")));
			SlotSelectBtn->SetStyle(Style);
			bSelectionState = false;
		}
	}
}

void UODObjectSlot::NativeDestruct()
{
	if(IsValid(SlotSelectBtn)){SlotSelectBtn->OnClicked.RemoveAll(this);}
	if(IsValid(ActivateCheckBox)){ActivateCheckBox->OnCheckStateChanged.RemoveAll(this);}
	
	Super::NativeDestruct();
}

FReply UODObjectSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		const auto CreatedMenu =  GetSlotContextMenu();
		const FWidgetPath WidgetPath = InMouseEvent.GetEventPath() != nullptr ? *InMouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(CreatedMenu->AsShared(), WidgetPath, CreatedMenu, InMouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}



TSharedRef<SWidget> UODObjectSlot::GetSlotContextMenu()
{	
	FMenuBuilder MenuBuilder(true, nullptr);

	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return SNullWidget::NullWidget;}

	const auto& SelectedSlotIndexes = ToolSubsystem->SelectedPaletteSlotIndexes;
	TArray<int32> RelatedIndexes;
	
	if(SelectedSlotIndexes.Num() == 0)
	{
		//Consider only clicked data
		RelatedIndexes.Add(SlotIndex);
	}
	else if(SelectedSlotIndexes.Contains(SlotIndex))
	{
		//Consider all selected assets
		RelatedIndexes = SelectedSlotIndexes;
	}
	else
	{
		OnSelectItems.ExecuteIfBound(false,{});
		
		//Deselect all and consider only clicked data
		RelatedIndexes.Add(SlotIndex);
	}

	//Get Related Object Data
	auto DistObjectData = ToolSubsystem->GetDistObjectDataWithIndexes(RelatedIndexes);

	//Collect Activated & Deactivated items
	bool IncludesActivated = false;
	bool IncludesDeactivated = false;
	for(const auto& CurrentData : DistObjectData)
	{
		if(CurrentData->ActiveStatus)
		{
			IncludesActivated = true;
		}
		else
		{
			IncludesDeactivated = true;
		}
	}
	
	auto SelectedDistObjectData = ToolSubsystem->GetSelectedDistObjectData();
	
	MenuBuilder.BeginSection("PaletteObjectHeadingMode", LOCTEXT("PaletteObjectHeading", "PALETTE OBJECT"));
	{
		if(IncludesDeactivated)
		{
			MenuBuilder.AddMenuEntry(
		  FText::FromString("Activate"),
		FText::FromString("Set this item as active."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([ToolSubsystem,RelatedIndexes]()
				{
					if(IsValid(ToolSubsystem))
					{
						ToolSubsystem->ActivateItemsWithIndex(true,RelatedIndexes);
					}
				}))
			);
		}
		if(IncludesActivated)
		{
			MenuBuilder.AddMenuEntry(
		  FText::FromString("Deactivate"),
		FText::FromString("Set this object as inactive."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([ToolSubsystem,RelatedIndexes]()
				{
					if(IsValid(ToolSubsystem))
					{
						ToolSubsystem->ActivateItemsWithIndex(false,RelatedIndexes);
					}
				}))
			);
		}
		
	MenuBuilder.AddMenuEntry(
  FText::FromString("Remove"),
FText::FromString("Remove this object from the palette"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([ToolSubsystem,RelatedIndexes, this]()
		{
			if(IsValid(ToolSubsystem))
			{
				OnRemoveSelectedItems.ExecuteIfBound(RelatedIndexes);
			}
		}))
	);

	MenuBuilder.AddMenuEntry(
  FText::FromString("Show In Content Browser"),
FText::FromString("Show static mesh in content browser"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([ToolSubsystem,RelatedIndexes]()
		{
			if(IsValid(ToolSubsystem))
			{
				ToolSubsystem->SyncPaletteItems(RelatedIndexes);
			}
		}))
	);
	}
	MenuBuilder.EndSection();

	
	MenuBuilder.BeginSection("PaletteSelectionMode", LOCTEXT("PaletteSelection", "SELECTION"));
	{
		if(!PresetName.IsNone())
		{
			MenuBuilder.AddMenuEntry(
		  FText::FromString("Select Preset"),
		FText::FromString("Select objects with the same preset only."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([this,ToolSubsystem]()
				{
					if(IsValid(ToolSubsystem))
					{
						const auto Presets = ToolSubsystem->GetIndexesOfPreset(PresetName);
						OnSelectItems.ExecuteIfBound(false,{});
						OnSelectItems.ExecuteIfBound(true,Presets);
					}
				}))
			);
		}
		
		MenuBuilder.AddMenuEntry(
		FText::FromString("Select All"),
		FText::FromString("Select all objects on the palette."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this, ToolSubsystem]()
			{
				if(IsValid(ToolSubsystem))
				{
					OnSelectItems.ExecuteIfBound(true,{});
				}
							
			}))
		);
	}
	MenuBuilder.EndSection();

	if(ToolSubsystem->SelectedPaletteSlotIndexes.Contains(SlotIndex))
	{
		MenuBuilder.BeginSection("SlotActions", LOCTEXT("ObjectActions", "Object Actions"));
		{
			MenuBuilder.AddWidget(
			SNew(SSlider)
			.ToolTipText( LOCTEXT("SpawnDensityToolTip", "Change the spawn denist of objects.") )
			.Value(0.5f)
			.OnValueChanged_UObject( this, &UODObjectSlot::SetSpawnDensity)
			//.IsEnabled( this, &SFoliagePalette::GetThumbnailScaleSliderEnabled)
			.OnMouseCaptureEnd_UObject(this, &UODObjectSlot::SliderCaptureEnd),
			LOCTEXT("LocalSpawnDensityLabel", "Spawn Density"),
			/*bNoIndent=#1#*/true
			);
		}

		MenuBuilder.EndSection();

		//boolSubsystem->StartDensitySession();
	}

	return MenuBuilder.MakeWidget();
}

void UODObjectSlot::OnActivateCheckBoxChanged(bool bInNewCondition)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}

	const auto& SelectedSlotIndexes = ToolSubsystem->SelectedPaletteSlotIndexes;
	if(SelectedSlotIndexes.Contains(SlotIndex))
	{
		ToolSubsystem->ActivateItemsWithIndex(bInNewCondition,SelectedSlotIndexes);
	}
	else
	{
		ToolSubsystem->ActivateItemsWithIndex(bInNewCondition,{SlotIndex});
	}
}

void UODObjectSlot::SetDetailsVisibility(const bool InbIsVisible)
{
	if(IsValid(ActivateCheckBox)){CheckBoxBorder->SetVisibility(InbIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);}
}

void UODObjectSlot::ChangeSlotDetailsVisibility(const bool InbIsMouseEnter)
{
	if(InbIsMouseEnter)
	{
		const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
		if(!ToolSubsystem){return;}

		const auto& SelectedSlotIndexes = ToolSubsystem->SelectedPaletteSlotIndexes;
		if(SelectedSlotIndexes.Contains(SlotIndex))
		{
			OnSlotDetailsVisibilityChanged.ExecuteIfBound(true,SelectedSlotIndexes);
		}
		else
		{
			OnSlotDetailsVisibilityChanged.ExecuteIfBound(true,{SlotIndex});
		}
	}
	else
	{
		OnSlotDetailsVisibilityChanged.ExecuteIfBound(false,{});
	}
}

void UODObjectSlot::SetSpawnDensity(float InDensity)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
	
	ToolSubsystem->ChangeSlotObjectDensity(InDensity);
}

void UODObjectSlot::SliderCaptureEnd()
{



}

void UODObjectSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem || !GEditor){return;}

	const TSoftClassPtr<UUserWidget> WidgetClassPtr(ODToolAssetData::PaletteToolTipPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto AssetToolTipMenu = Cast<UODPaletteItemToolTip>(CreateWidget(GEditor->GetEditorWorldContext().World(), ClassRef)))
		{
			if(ToolSubsystem->ObjectDistributionData.IsValidIndex(SlotIndex))
			{
				const auto AssetData = UODAssetFunctions::GetAssetDataFromPath(ToolSubsystem->ObjectDistributionData[SlotIndex].StaticMesh.ToSoftObjectPath().ToString());
				AssetToolTipMenu->SetTooltipData(PresetName,AssetData);
				SlotSelectBtn->SetToolTip(AssetToolTipMenu);
			}
		}
	}

	ChangeSlotDetailsVisibility(true);
}

void UODObjectSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	ChangeSlotDetailsVisibility(false);
	
	Super::NativeOnMouseLeave(InMouseEvent);
}



#undef LOCTEXT_NAMESPACE
