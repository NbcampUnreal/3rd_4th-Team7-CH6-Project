// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.



#include "ASSplineSlot.h"
#include "EngineUtils.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"


#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Subsystems/EditorActorSubsystem.h"


void UASSplineSlot::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (!SeeSplineBtn->OnClicked.IsBound())
	{
		SeeSplineBtn->OnClicked.AddDynamic(this, &UASSplineSlot::SeeSplineBtnPressed);
	}

	if (!RenameBtn->OnClicked.IsBound())
	{
		RenameBtn->OnClicked.AddDynamic(this, &UASSplineSlot::RenameBtnPressed);
	}

	if (!SelectionCheckbox->OnCheckStateChanged.IsBound())
	{
		SelectionCheckbox->OnCheckStateChanged.AddDynamic(this, &UASSplineSlot::SelectionCheckboxChanged);
	}

	if (!SelectionCheckbox->OnCheckStateChanged.IsBound())
	{
		SelectionCheckbox->OnCheckStateChanged.AddDynamic(this, &UASSplineSlot::SelectionCheckboxChanged);
	}

	if (!SplineNameTextBox->OnTextCommitted.IsBound())
	{
		SplineNameTextBox->OnTextCommitted.AddDynamic(this,&UASSplineSlot::SplineNameTextBoxCommitted);
	}	

	SelectionCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	
	SplineNameTextBox->SetIsEnabled(false);


	SeeSplineBtn->SetToolTipText(FText::FromName(TEXT("Focuse on the actor in the level.")));
	RenameBtn->SetToolTipText(FText::FromName(TEXT("Rename the spline actor.")));
}

void UASSplineSlot::NativeDestruct()
{
	SeeSplineBtn->OnClicked.RemoveAll(this);
	RenameBtn->OnClicked.RemoveAll(this);
	SelectionCheckbox->OnCheckStateChanged.RemoveAll(this);
	SplineNameTextBox->OnTextCommitted.RemoveAll(this);


	
	Super::NativeDestruct();
}

void UASSplineSlot::SetSplineData(const FString& InLabel,const FString& InObjectName)
{
	SplineObjectLabel = InLabel;
	SplineNameTextBox->SetText(FText::FromString(SplineObjectLabel));
	SplineObjectName = InObjectName;
}

void UASSplineSlot::SeeSplineBtnPressed()
{
	if(SplineObjectName.IsEmpty()){return;}
	
	if(GUnrealEd)
	{
		const TObjectPtr<AActor> FoundActor =  GetActorWithObjectName(SplineObjectName);
		if(!FoundActor){UE_LOG(LogTemp,Error,TEXT("Not Found The Spline Actor!")); return;}

		if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
		{
			EditorActorSubsystem->SelectNothing();
			EditorActorSubsystem->SetActorSelectionState(FoundActor,true);
			GUnrealEd->Exec(FoundActor->GetWorld(), TEXT("CAMERA ALIGN ACTIVEVIEWPORTONLY"));
		}
	}
	
}

void UASSplineSlot::RenameBtnPressed()
{
	RenameBtn->SetIsEnabled(false);
	SplineNameTextBox->SetIsEnabled(true);
	SplineNameTextBox->SetText(FText::GetEmpty());
	SplineNameTextBox->SetKeyboardFocus();
}

void UASSplineSlot::SplineNameTextBoxCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if(InCommitMethod == ETextCommit::Type::OnEnter)
	{
		if(!InText.IsEmpty() || InText.EqualTo(FText::FromString(SplineObjectLabel)))
		{
			if(GUnrealEd)
			{
				AActor* FoundActor =  GetActorWithObjectName(SplineObjectName);

				SplineObjectLabel = SplineNameTextBox->GetText().ToString();
				FoundActor->SetActorLabel(SplineObjectLabel);
			}
		}
	}
	SplineNameTextBox->SetText(FText::FromString(SplineObjectLabel));
	RenameBtn->SetIsEnabled(true);
	SplineNameTextBox->SetIsEnabled(false);
}

void UASSplineSlot::SelectionCheckboxChanged(bool InNewState)
{
	OnSplineSlotCheckboxStateChanged.ExecuteIfBound(InNewState);
}

void UASSplineSlot::SetSelectionState(const bool& bInNewState) const
{
	SelectionCheckbox->SetCheckedState(bInNewState ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

bool UASSplineSlot::GetSelectionState() const
{
	return SelectionCheckbox->GetCheckedState() == ECheckBoxState::Checked ? true : false;
}

AActor* UASSplineSlot::GetActorWithObjectName(const FString& InName)
{
	if(const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
	{
		for (TActorIterator<AActor> ActorItr(EditorWorld); ActorItr; ++ActorItr)
		{
			AActor* LocalActor = *ActorItr;

			if(IsValid(LocalActor) && LocalActor->GetName().Equals(InName))
			{
				return LocalActor;
			}
		}
	}
	
	return nullptr;
}
