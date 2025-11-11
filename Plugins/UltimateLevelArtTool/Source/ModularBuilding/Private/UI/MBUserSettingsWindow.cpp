// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBUserSettingsWindow.h"
#include "Editor.h"
#include "MBToolSubsystem.h"
#include "MBUserSettingsObject.h"
#include "Components/Button.h"
#include "Components/DetailsView.h"

void UMBUserSettingsWindow::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR
	
	if((UserSettingsObject = Cast<UMBUserSettingsObject>(NewObject<UMBUserSettingsObject>(this, TEXT("UserSettings")))))
	{
		UserSettingsObject->InitializeSettings();
		UserSettingsDetails->SetObject(UserSettingsObject);
	}
#endif // WITH_EDITOR
}

void UMBUserSettingsWindow::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsValid(ClearTheSceneBtn) || !IsValid(ClearTheSceneBtn)){return;}
	
	if (!ClearTheSceneBtn->OnClicked.IsBound())
	{
		ClearTheSceneBtn->OnClicked.AddDynamic(this, &UMBUserSettingsWindow::ClearTheSceneBtnPressed);
	}
	
	if (!ResetToolBtn->OnClicked.IsBound())
	{
		ResetToolBtn->OnClicked.AddDynamic(this, &UMBUserSettingsWindow::ResetToolBtnPressed);
	}

	ResetToolBtn->SetToolTipText(FText::FromName(TEXT("Destroys all modular actors at the level.")));
	ClearTheSceneBtn->SetToolTipText(FText::FromName(TEXT("Resets the interface to its default settings.")));
}

void UMBUserSettingsWindow::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(ResetToolBtn)) { ResetToolBtn->OnClicked.RemoveAll(this); }
	if (IsValid(ClearTheSceneBtn)) { ClearTheSceneBtn->OnClicked.RemoveAll(this); }
}

void UMBUserSettingsWindow::ResetToolBtnPressed()
{
	if (const auto ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		ToolSettings->ResetTool();
	}
	if(IsValid(UserSettingsObject))
	{
		UserSettingsObject->InitializeSettings();
	}
}

void UMBUserSettingsWindow::ClearTheSceneBtnPressed()
{
	if(const auto ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>())
	{
		ToolSettings->DeleteAllModularActorOnTheLevel();
	}
}
