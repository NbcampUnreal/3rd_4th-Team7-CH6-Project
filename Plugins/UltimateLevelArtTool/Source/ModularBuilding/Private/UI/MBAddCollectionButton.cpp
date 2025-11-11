// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBAddCollectionButton.h"
#include "Editor.h"
#include "Components/Button.h"
#include "MBToolSubsystem.h"
#include "Interfaces/MBMainScreenInterface.h"

void UMBAddCollectionButton::NativeConstruct()
{
	Super::NativeConstruct();

	AddCollectionBtn->SetToolTipText(FText::FromString(TEXT("Add New Collection")));

	if (!AddCollectionBtn->OnClicked.IsBound())
	{
		AddCollectionBtn->OnClicked.AddDynamic(this, &UMBAddCollectionButton::AddCollectionBtnPressed);
	}


	
}

void UMBAddCollectionButton::NativeDestruct()
{
	if(AddCollectionBtn){AddCollectionBtn->OnClicked.RemoveAll(this);}
	
	Super::NativeDestruct();
}

void UMBAddCollectionButton::AddCollectionBtnPressed()
{
	
	if(const auto ToolMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
	{
		Cast<IMBMainScreenInterface>(ToolMainScreen)->AddNewCollection();
	}
}
