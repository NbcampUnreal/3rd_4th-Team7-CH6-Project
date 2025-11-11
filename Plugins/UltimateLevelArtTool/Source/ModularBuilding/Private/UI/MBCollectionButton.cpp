// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBCollectionButton.h"
#include "Editor.h"
#include "MBUserSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Data/MBToolData.h"
#include "MBToolSubsystem.h"
#include "Development/MBDebug.h"
#include "Interfaces/MBMainScreenInterface.h"


void UMBCollectionButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!ModularCollectionBtn->OnClicked.IsBound())
	{
		ModularCollectionBtn->OnClicked.AddDynamic(this, &UMBCollectionButton::ModularCollectionBtnPressed);

		ModularCollectionBtn->SetToolTipText(FText::FromName(TEXT("Click to activate or deactivate the collection.\nDouble-click to open or close the collection window.")));
	}
}

void UMBCollectionButton::NativeDestruct()
{
	if(ModularCollectionBtn){ModularCollectionBtn->OnClicked.RemoveAll(this);}
	
	Super::NativeDestruct();
}


void UMBCollectionButton::SetCollection(const FName InName) 
{
	CollectionName = InName;
	CollectionText->SetText(FText::FromString(InName.ToString()));

	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		if(ToolSettingsSubsystem->LastActiveCollectionIndex >= 0)
		{
			if(ToolSettingsSubsystem->GetToolData()->Collections[ToolSettingsSubsystem->LastActiveCollectionIndex].IsEqual(InName))
			{
				SetSelectionState(true);
			}
		}
	}
}

void UMBCollectionButton::ModularCollectionBtnPressed()
{
	CheckForDoubleClick();
	
}


void UMBCollectionButton::CheckForDoubleClick()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(DoubleClickTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DoubleClickTimerHandle);
		OnDoubleClickCheckingFinished(true);
	}
	else
	{
		 GetWorld()->GetTimerManager().SetTimer(DoubleClickTimerHandle, this, &UMBCollectionButton::OnTimerFinished, 0.3f, false);
	}
}

void UMBCollectionButton::OnTimerFinished()
{
	OnDoubleClickCheckingFinished(false);
	GetWorld()->GetTimerManager().ClearTimer(DoubleClickTimerHandle);
}

void UMBCollectionButton::OnDoubleClickCheckingFinished(const bool bInIsDoubleClicked)
{
	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSettingsSubsystem){return;}
	
	if(bInIsDoubleClicked)
	{
		const auto ToolMainScreen = ToolSettingsSubsystem->GetToolMainScreen();
		if(!ToolMainScreen.Get()){return;}
		
		if(ToolSettingsSubsystem->ActiveCollectionWindow.IsEqual(CollectionName))
		{
			ToolSettingsSubsystem->LastActiveCollectionIndex = ToolSettingsSubsystem->GetToolData()->Collections.Find(CollectionName);
			SetSelectionState(true);
			ToolSettingsSubsystem->ActiveCollectionWindow = FName();
			Cast<IMBMainScreenInterface>(ToolMainScreen)->UpdateTheCollectionTabState();
			
		}
		else //Open Collection Window
		{
			if(const auto MainScreenInterface = Cast<IMBMainScreenInterface>(GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen()))
			{
				MainScreenInterface->TurnOffCollectionState(ToolSettingsSubsystem->LastActiveCollectionIndex);
			}
			
			ToolSettingsSubsystem->LastActiveCollectionIndex = ToolSettingsSubsystem->GetToolData()->Collections.Find(CollectionName);
			SetSelectionState(true);
			ToolSettingsSubsystem->ActiveCollectionWindow = CollectionName;
			Cast<IMBMainScreenInterface>(ToolMainScreen)->UpdateTheCollectionTabState();
		}
	}
	else
	{
		if(const auto ToolData = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolData())
		{
			if(ToolSettingsSubsystem->LastActiveCollectionIndex < 0)
			{
				ToolSettingsSubsystem->LastActiveCollectionIndex = ToolData->Collections.Find(CollectionName);
				SetSelectionState(true);
			}
			else if(ToolSettingsSubsystem->LastActiveCollectionIndex == ToolData->Collections.Find(CollectionName))
			{
				ToolSettingsSubsystem->LastActiveCollectionIndex = -1;
				SetSelectionState(false);

				if(!ToolSettingsSubsystem->ActiveCollectionWindow.IsNone() && ToolSettingsSubsystem->ActiveCollectionWindow == CollectionName)
				{
					ToolSettingsSubsystem->ActiveCollectionWindow = FName();
					if(const auto ToolScreenInterface = Cast<IMBMainScreenInterface>(GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen()))
					{
						ToolScreenInterface->UpdateTheCollectionTabState();
					}
				}
				
			}
			else //Change Another Buttons State
			{
				if(const auto ToolScreenInterface = Cast<IMBMainScreenInterface>(GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen()))
				{
					ToolScreenInterface->TurnOffCollectionState(ToolSettingsSubsystem->LastActiveCollectionIndex);
				}

				if(!ToolSettingsSubsystem->ActiveCollectionWindow.IsNone() && ToolSettingsSubsystem->ActiveCollectionWindow != CollectionName)
				{
					ToolSettingsSubsystem->ActiveCollectionWindow = FName();
					if(const auto ToolScreenInterface = Cast<IMBMainScreenInterface>(GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen()))
					{
						ToolScreenInterface->UpdateTheCollectionTabState();
					}
				}
				
				ToolSettingsSubsystem->LastActiveCollectionIndex = ToolData->Collections.Find(CollectionName);
				SetSelectionState(true);
			}
		}
	}
}

void UMBCollectionButton::SetSelectionState(const bool InSelectionState) const
{
	if(InSelectionState)
	{
		if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
		{
			if(const auto UserSettingsData = ToolSettingsSubsystem->GetToolUserSettings())
			{
				FButtonStyle Style = ModularCollectionBtn->GetStyle();
				Style.Normal.TintColor = UserSettingsData->CollectionSelectionColor;
				Style.Hovered.TintColor = UserSettingsData->CollectionSelectionColor;
				Style.Pressed.TintColor = UserSettingsData->CollectionSelectionColor;
				Style.Disabled.TintColor = UserSettingsData->CollectionSelectionColor;
				ModularCollectionBtn->SetStyle(Style);
			}
		}
	}
	else
	{
		FButtonStyle Style = ModularCollectionBtn->GetStyle();
		Style.Normal.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("010101FF")));
		Style.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("282828FF")));
		Style.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("010101FF")));
		Style.Disabled.TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("010101FF")));
		ModularCollectionBtn->SetStyle(Style);
	}
}

void UMBCollectionButton::ChangeCollectionName(const FName& InCollectionName)
{
	CollectionName = InCollectionName;
	CollectionText->SetText(FText::FromString(InCollectionName.ToString()));
}
