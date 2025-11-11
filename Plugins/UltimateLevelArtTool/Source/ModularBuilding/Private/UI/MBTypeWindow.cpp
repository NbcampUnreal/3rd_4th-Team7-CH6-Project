// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBTypeWindow.h"
#include "Components/ScrollBox.h"
#include "Engine/StaticMesh.h"
#include "UI/MBAssetSlot.h"
#include "Editor.h"
#include "MBMainScreenInterface.h"
#include "Components/Button.h"
#include "MBToolAssetData.h"
#include "MBToolSubsystem.h"
#include "Development/MBDebug.h"
#include "Components/EditableTextBox.h"
#include "Data/MBToolData.h"

void UMBTypeWindow::NativeConstruct()
{
	Super::NativeConstruct();

	if(AssetDropBorder){AssetDropBorder->GetOnAssetDroppedSignature()->BindUObject(this,&UMBTypeWindow::OnAssetDropped);}

	if(!TitleTextBox->OnTextCommitted.IsBound())
	{
		TitleTextBox->OnTextCommitted.AddDynamic(this,&UMBTypeWindow::OnTitleTextBoxCommitted);
	}
	if (!TypeUpBtn->OnClicked.IsBound())
	{
		TypeUpBtn->OnClicked.AddDynamic(this, &UMBTypeWindow::TypeUpBtnPressed);
	}
	if (!TypeDownBtn->OnClicked.IsBound())
	{
		TypeDownBtn->OnClicked.AddDynamic(this, &UMBTypeWindow::TypeDownBtnPressed);
	}
	if (!DeleteBtn->OnClicked.IsBound())
	{
		DeleteBtn->OnClicked.AddDynamic(this, &UMBTypeWindow::DeleteBtnPressed);
	}
	DeleteBtn->SetVisibility(ESlateVisibility::Hidden);
	TypeUpBtn->SetVisibility(ESlateVisibility::Hidden);
	TypeDownBtn->SetVisibility(ESlateVisibility::Hidden);
	DeleteBtn->SetToolTipText(FText::FromString(TEXT("Remove this asset type from the tool \n and any associated assets.")));
	TypeUpBtn->SetToolTipText(FText::FromString(TEXT("Move this asset type up one row.")));
	TypeDownBtn->SetToolTipText(FText::FromString(TEXT("Move this asset type down one row.")));
}

void UMBTypeWindow::NativeDestruct()
{
	if (TitleTextBox) { TitleTextBox->OnTextCommitted.RemoveAll(this); }
	if (DeleteBtn) { DeleteBtn->OnClicked.RemoveAll(this); }
	
	Super::NativeDestruct();
}

void UMBTypeWindow::SetMenuType(const FName& InType)
{
	MenuType = InType;
	TitleTextBox->SetText(FText::FromName(InType));
}

void UMBTypeWindow::AddSlotToMenu(const UStaticMesh* InAsset)
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	
	const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ModularAssetSlotPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto NewSlot = Cast<UMBAssetSlot>(CreateWidget(EditorWorld, ClassRef)))
		{
			AddedSlots.Add(NewSlot);
			NewSlot->SetSlotParams(InAsset);
			AssetBox->AddChild(NewSlot);
		}
	}
}

void UMBTypeWindow::OnTitleTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(Text.EqualTo(FText::FromName(MenuType))){return;}
	
	if(Text.IsEmpty())
	{
		TitleTextBox->SetText(FText::FromName(MenuType));
		return;
	}
	if(const auto ToolSettingsSubsystem  = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->ChangeAssetType(MenuType,FName(*Text.ToString()),ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory))
		{
			const FString Dialog = FString::Printf(TEXT("Asset type changed succesfully"));
			MBDebug::ShowNotifyInfo(Dialog);
		}
	}
}

void UMBTypeWindow::TypeUpBtnPressed()
{
	if(const auto ToolSettingsSubsystem  = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->ChangeTypeOrder(MenuType,ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory,true))
		{
			if(const auto IMBToolWindow = Cast<IMBMainScreenInterface>(ToolSettingsSubsystem->GetToolMainScreen()))
			{
				IMBToolWindow->UpdateCategoryWindow();
			}
		}
	}
}

void UMBTypeWindow::TypeDownBtnPressed()
{
	if(const auto ToolSettingsSubsystem  = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->ChangeTypeOrder(MenuType,ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory,false))
		{
			if(const auto IMBToolWindow = Cast<IMBMainScreenInterface>(ToolSettingsSubsystem->GetToolMainScreen()))
			{
				IMBToolWindow->UpdateCategoryWindow();
			}
		}
	}
}

void UMBTypeWindow::DeleteBtnPressed()
{	
	if(const auto ToolSettingsSubsystem  = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->RemoveAssetType(MenuType,ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory))
		{
			const FString Dialog = FString::Printf(TEXT("Asset type removed succesfully"));
			MBDebug::ShowNotifyInfo(Dialog);
		}
	}
}

void UMBTypeWindow::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	DeleteBtn->SetVisibility(ESlateVisibility::Visible);

	if(const auto ToolSettingsSubsystem  = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
		{
			const int32 FoundIndex = ToolSettingsSubsystem->GetToolData()->ModularMeshTypes.Find(MenuType);
			if(FoundIndex > 0)
			{
				TypeUpBtn->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				TypeUpBtn->SetVisibility(ESlateVisibility::Collapsed);
			}
			if(FoundIndex < ToolSettingsSubsystem->GetToolData()->ModularMeshTypes.Num() - 1)
			{
				TypeDownBtn->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				TypeDownBtn->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else if(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Prop)
		{
			const int32 FoundIndex = ToolSettingsSubsystem->GetToolData()->PropMeshTypes.Find(MenuType);
			if(FoundIndex > 0)
			{
				TypeUpBtn->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				TypeUpBtn->SetVisibility(ESlateVisibility::Collapsed);
			}
			if(FoundIndex < ToolSettingsSubsystem->GetToolData()->PropMeshTypes.Num() - 1)
			{
				TypeDownBtn->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				TypeDownBtn->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UMBTypeWindow::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	DeleteBtn->SetVisibility(ESlateVisibility::Hidden);
	TypeUpBtn->SetVisibility(ESlateVisibility::Hidden);
	TypeDownBtn->SetVisibility(ESlateVisibility::Hidden);
}

void UMBTypeWindow::ResetAllSelectionStates()
{
	if(AddedSlots.IsEmpty()){return;}

	for(const auto LocalSlot : AddedSlots)
	{
		LocalSlot->SetSelectionState(false);
	}
}

void UMBTypeWindow::OnAssetDropped(TArrayView<FAssetData> DroppedAssets) const
{
	if(const auto ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		ToolSettings->AddNewAssetsToTheTool(MenuType,DroppedAssets);
	}
}


	

	

