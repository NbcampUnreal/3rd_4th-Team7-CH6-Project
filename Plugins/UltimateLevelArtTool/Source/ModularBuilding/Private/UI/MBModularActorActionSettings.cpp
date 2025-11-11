// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBModularActorActionSettings.h"
#include "Editor.h"
#include "MBMActionSettingsObj.h"
#include "MBToolSubsystem.h"
#include "Components/Button.h"
#include "Interfaces/MBMainScreenInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Components/Border.h"
#include "MBToolAssetData.h"
#include "Development/MBDebug.h"
#include "Interfaces/MBBuildingManagerInterface.h"
#include "UI/MBMaterialAssignmentWindow.h"
#include "Components/DetailsView.h"


void UMBModularActorActionSettings::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR

	if(const auto ActorActionSettingsObj = Cast<UMBMActionSettingsObj>(NewObject<UMBMActionSettingsObj>(this, TEXT("ModularBuildingSettingsObj"))))
	{
		ModActorActionDetails->SetObject(ActorActionSettingsObj);
	}

#endif
}

void UMBModularActorActionSettings::NativeConstruct()
{
	Super::NativeConstruct();
	
	RegenerateDuplicationMenu();
	
	if (!ApplyBtn->OnClicked.IsBound())
	{
		ApplyBtn->OnClicked.AddDynamic(this, &UMBModularActorActionSettings::ApplyBtnPressed);
	}
	if (!ResetDupBtn->OnClicked.IsBound())
	{
		ResetDupBtn->OnClicked.AddDynamic(this, &UMBModularActorActionSettings::ResetDupBtnPressed);
	}
	
	if(!XDupText->OnTextCommitted.IsBound())
	{
		XDupText->OnTextCommitted.AddDynamic(this,&UMBModularActorActionSettings::OnXDupTextCommitted);
	}

	if(!YDupText->OnTextCommitted.IsBound())
	{
		YDupText->OnTextCommitted.AddDynamic(this,&UMBModularActorActionSettings::OnYDupTextCommitted);
	}

	if(!ZDupText->OnTextCommitted.IsBound())
	{
		ZDupText->OnTextCommitted.AddDynamic(this,&UMBModularActorActionSettings::OnZDupTextCommitted);
	}

	if(!XDupOffset->OnTextCommitted.IsBound())
	{
		XDupOffset->OnTextCommitted.AddDynamic(this,&UMBModularActorActionSettings::OnXDupOffsetCommitted);
	}

	if(!YDupOffset->OnTextCommitted.IsBound())
	{
		YDupOffset->OnTextCommitted.AddDynamic(this,&UMBModularActorActionSettings::OnYDupOffsetCommitted);
	}
	
	if(!ZDupOffset->OnTextCommitted.IsBound())
	{
		ZDupOffset->OnTextCommitted.AddDynamic(this,&UMBModularActorActionSettings::OnZDupOffsetCommitted);
	}
	if(!XDirCheck->OnCheckStateChanged.IsBound())
	{
		XDirCheck->OnCheckStateChanged.AddDynamic(this,&UMBModularActorActionSettings::OnXDirCheckChanged);
	}
	if(!YDirCheck->OnCheckStateChanged.IsBound())
	{
		YDirCheck->OnCheckStateChanged.AddDynamic(this,&UMBModularActorActionSettings::OnYDirCheckChanged);
	}
	if(!ZDirCheck->OnCheckStateChanged.IsBound())
	{
		ZDirCheck->OnCheckStateChanged.AddDynamic(this,&UMBModularActorActionSettings::OnZDirCheckChanged);
	}

	if(!HoleCheck->OnCheckStateChanged.IsBound())
	{
		HoleCheck->OnCheckStateChanged.AddDynamic(this,&UMBModularActorActionSettings::OnHoleCheckChanged);
	}

	if(!RectangleCheck->OnCheckStateChanged.IsBound())
	{
		RectangleCheck->OnCheckStateChanged.AddDynamic(this,&UMBModularActorActionSettings::OnRectangleCheckChanged);
	}
	
	//Add Material Assignment Tool
	if (!GEditor){return;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::MaterialAssignmentWindowPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto CreatedMaterialAssignmentWindow = Cast<UMBMaterialAssignmentWindow>(CreateWidget(EditorWorld,ClassRef)))
		{
			if(CreatedMaterialAssignmentWindow->RedesignTheMaterialWindow())
			{
				MaterialAssignmentBox->AddChild(CreatedMaterialAssignmentWindow);
			}
			else
			{
				CreatedMaterialAssignmentWindow->Destruct();
				MaterialAssignmentBox->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UMBModularActorActionSettings::NativeDestruct()
{
	if (ApplyBtn) { ApplyBtn->OnClicked.RemoveAll(this); }
	if (ResetDupBtn) { ResetDupBtn->OnClicked.RemoveAll(this); }
	if (XDupText) { XDupText->OnTextCommitted.RemoveAll(this); }
	if (YDupText) { YDupText->OnTextCommitted.RemoveAll(this); }
	if (ZDupText) { ZDupText->OnTextCommitted.RemoveAll(this); }
	if (XDupOffset) { XDupOffset->OnTextCommitted.RemoveAll(this); }
	if (YDupOffset) { YDupOffset->OnTextCommitted.RemoveAll(this); }
	if (ZDupOffset) { ZDupOffset->OnTextCommitted.RemoveAll(this); }
	if (XDirCheck) { XDirCheck->OnCheckStateChanged.RemoveAll(this); }
	if (YDirCheck) { YDirCheck->OnCheckStateChanged.RemoveAll(this); }
	if (ZDirCheck) { ZDirCheck->OnCheckStateChanged.RemoveAll(this); }
	if (HoleCheck) { HoleCheck->OnCheckStateChanged.RemoveAll(this); }
	if (RectangleCheck) { RectangleCheck->OnCheckStateChanged.RemoveAll(this); }
	
	Super::NativeDestruct();
}


void UMBModularActorActionSettings::ApplyBtnPressed()
{
	if(const auto ToolMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
	{
		Cast<IMBMainScreenInterface>(ToolMainScreen)->ApplyModularDuplicationPressed();
	}
}

void UMBModularActorActionSettings::ResetDupBtnPressed()
{
	if(const auto IMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
	{
		if(const auto IBuildingManager = Cast<IMBBuildingManagerInterface>(Cast<IMBMainScreenInterface>(IMainScreen)->GetBuildingManager()))
		{
			IBuildingManager->ResetModularDuplication();
		}
		RegenerateDuplicationMenu();
	}
}



void UMBModularActorActionSettings::OnXDupTextCommitted(const FText& Text,ETextCommit::Type CommitMethod)
{
	CheckForNumOfDupCommit(XDuplicationData,Text,XDupText);
}

void UMBModularActorActionSettings::OnYDupTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	CheckForNumOfDupCommit(YDuplicationData,Text,YDupText);
}

void UMBModularActorActionSettings::OnZDupTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	CheckForNumOfDupCommit(ZDuplicationData,Text,ZDupText);
}

void UMBModularActorActionSettings::OnXDupOffsetCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	CheckForOffsetCommit(XDuplicationData,Text,XDupOffset);
}

void UMBModularActorActionSettings::OnYDupOffsetCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	CheckForOffsetCommit(YDuplicationData,Text,YDupOffset);
}

void UMBModularActorActionSettings::OnZDupOffsetCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	CheckForOffsetCommit(ZDuplicationData,Text,ZDupOffset);
}

void UMBModularActorActionSettings::OnXDirCheckChanged(bool bInNewCondition)
{
	CheckForDirCommit(XDuplicationData,bInNewCondition);
}

void UMBModularActorActionSettings::OnYDirCheckChanged(bool bInNewCondition)
{
	CheckForDirCommit(YDuplicationData,bInNewCondition);
}

void UMBModularActorActionSettings::OnZDirCheckChanged(bool bInNewCondition)
{
	CheckForDirCommit(ZDuplicationData,bInNewCondition);
}

void UMBModularActorActionSettings::OnHoleCheckChanged(bool bInNewCondition)
{
	if(DuplicationFilters.Hole != bInNewCondition)
	{
		if(bInNewCondition && DuplicationFilters.Rectangle)
		{
			RectangleCheck->SetCheckedState(ECheckBoxState::Unchecked);
			DuplicationFilters.Rectangle = false;
		}
		DuplicationFilters.Hole = bInNewCondition;
		if(const auto ToolMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
		{
			Cast<IMBMainScreenInterface>(ToolMainScreen)->ApplyModularDuplicationFilter(DuplicationFilters);
		}
	}
}

void UMBModularActorActionSettings::OnRectangleCheckChanged(bool bInNewCondition)
{
	if(DuplicationFilters.Rectangle != bInNewCondition)
	{
		if(bInNewCondition && DuplicationFilters.Hole)
		{
			HoleCheck->SetCheckedState(ECheckBoxState::Unchecked);
			DuplicationFilters.Hole = false;
		}
		DuplicationFilters.Rectangle = bInNewCondition;
		
		if(const auto ToolMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
		{
			Cast<IMBMainScreenInterface>(ToolMainScreen)->ApplyModularDuplicationFilter(DuplicationFilters);
		}
	}
}

void UMBModularActorActionSettings::CheckForNumOfDupCommit(FDuplicationData& InDuplicationData,const FText& InText, UEditableTextBox* INDupBox) const
{
	if( InText.ToString().IsNumeric())
	{
		const int32 NewNum = FCString::Atoi(*InText.ToString());
		if(NewNum > 0 && NewNum != InDuplicationData.NumOfDup)
		{
			InDuplicationData.NumOfDup = NewNum;
			RegenerateDuplication(InDuplicationData);
			return;
		}
	}
	INDupBox->SetText(FText::FromString(FString::FromInt(InDuplicationData.NumOfDup)));
}

void UMBModularActorActionSettings::CheckForOffsetCommit(FDuplicationData& InDuplicationData, const FText& InText,UEditableTextBox* INDupBox) const
{
	if( InText.ToString().IsNumeric())
	{
		const auto NewNum = FCString::Atof(*InText.ToString());
		if(/*NewNum >= 0.0f && */ NewNum != InDuplicationData.Offset)
		{
			InDuplicationData.Offset = NewNum;
			RegenerateDuplication(InDuplicationData);
		}
		return;
	}
	INDupBox->SetText(FText::FromString(FString::FromInt(InDuplicationData.Offset)));
}

void UMBModularActorActionSettings::CheckForDirCommit(FDuplicationData& InDuplicationData, const bool& bInNewBool) const
{
	if(InDuplicationData.bDirection != bInNewBool)
	{
		InDuplicationData.bDirection = bInNewBool;
		RegenerateDuplication(InDuplicationData);
	}
}


void UMBModularActorActionSettings::RegenerateDuplicationMenu()
{
	if(const auto ToolMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
	{
		auto DupData = Cast<IMBMainScreenInterface>(ToolMainScreen)->GetExistingDuplicationData();

		for(const auto CurrentData : DupData)
		{
			if(CurrentData->DupAxis == XDuplicationData.DupAxis)
			{
				XDuplicationData = *CurrentData;
			}
			else if(CurrentData->DupAxis == YDuplicationData.DupAxis)
			{
				YDuplicationData = *CurrentData;
			}
			else if(CurrentData->DupAxis == ZDuplicationData.DupAxis)
			{
				ZDuplicationData = *CurrentData;
			}
		}
		const auto DupFilter = Cast<IMBMainScreenInterface>(ToolMainScreen)->GetExistingDuplicationFilter();
		DuplicationFilters.Hole = DupFilter->Hole;
		DuplicationFilters.Rectangle = DupFilter->Rectangle;
	}
	
	XDupText->SetText(FText::FromString(FString::FromInt(XDuplicationData.NumOfDup)));
	YDupText->SetText(FText::FromString(FString::FromInt(YDuplicationData.NumOfDup)));
	ZDupText->SetText(FText::FromString(FString::FromInt(ZDuplicationData.NumOfDup)));
	XDupOffset->SetText(FText::FromString(FString::SanitizeFloat(XDuplicationData.Offset)));
	YDupOffset->SetText(FText::FromString(FString::SanitizeFloat(YDuplicationData.Offset)));
	ZDupOffset->SetText(FText::FromString(FString::SanitizeFloat(ZDuplicationData.Offset)));
	XDirCheck->SetCheckedState(XDuplicationData.bDirection ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	YDirCheck->SetCheckedState(YDuplicationData.bDirection ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	ZDirCheck->SetCheckedState(ZDuplicationData.bDirection ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	HoleCheck->SetCheckedState(DuplicationFilters.Hole ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	RectangleCheck->SetCheckedState(DuplicationFilters.Hole ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

}


void UMBModularActorActionSettings::RegenerateDuplication(const FDuplicationData& InDuplicationData) const
{
	if(const auto ToolMainScreen = GEditor->GetEditorSubsystem<UMBToolSubsystem>()->GetToolMainScreen().Get())
	{
		Cast<IMBMainScreenInterface>(ToolMainScreen)->StartModularDuplicationFromSettings(InDuplicationData);
	}
}



