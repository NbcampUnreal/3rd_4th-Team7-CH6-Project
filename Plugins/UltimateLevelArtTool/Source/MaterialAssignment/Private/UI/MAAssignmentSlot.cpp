// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MAAssignmentSlot.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "MADebug.h"
#include "MAFunctions.h"
#include "MAMaterialParamObject.h"
#include "MAToolSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/SinglePropertyView.h"
#include "Components/PropertyViewBase.h"
#include "Components/TextBlock.h"
#include "Interfaces/MAToolWindowInterface.h"
#include "Materials/MaterialInterface.h"

class FAssetRegistryModule;

void UMAAssignmentSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(IsValid(ApplyBtn)){ApplyBtn->SetIsEnabled(false);}
	if(IsValid(MultipleChoiceBox)){MultipleChoiceBox->SetToolTipText(FText::FromName(TEXT("The static meshes with this slot contain different materials.")));}
	if(IsValid(CounterText)){CounterText->SetToolTipText(FText::FromName(TEXT("This value indicates the number of static meshes with this slot.")));}
	if(IsValid(SlotRenameText)){SlotRenameText->SetToolTipText(FText::FromName(TEXT("Click to change the name of this slot in all meshes that have this slot.")));}
}

void UMAAssignmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ApplyBtn) && !ApplyBtn->OnClicked.IsBound())
	{
		ApplyBtn->OnClicked.AddDynamic(this, &UMAAssignmentSlot::ApplyBtnPressed);
	}

	if (IsValid(QuickAssignBtn) && !QuickAssignBtn->OnClicked.IsBound())
	{
		QuickAssignBtn->OnClicked.AddDynamic(this, &UMAAssignmentSlot::QuickAssignBtnPressed);
	}

	if (IsValid(DeleteBtn) && !DeleteBtn->OnClicked.IsBound())
	{
		DeleteBtn->OnReleased.AddDynamic(this, &UMAAssignmentSlot::DeleteBtnPressed);
	}

	if(IsValid(SlotRenameText) && !SlotRenameText->OnTextCommitted.IsBound())
	{
		SlotRenameText->OnTextCommitted.AddDynamic(this,&UMAAssignmentSlot::OnSlotNameTextCommitted);
	}
	
}

void UMAAssignmentSlot::InitializeTheSlot(const FName& InSlotName, UMaterialInterface* InExistingMaterial,const FString& InFirstMeshName,const FAssetData& InSuggestedMaterial,bool IsSlotInUse)
{
	SlotName = InSlotName;
	MeshNames.Add(InFirstMeshName);
	SlotRenameText->SetText(FText::FromName(InSlotName));
	if(IsValid(InExistingMaterial))
	{
		DefaultMaterialName = InExistingMaterial->GetName();
	}

	if((MaterialParamObject = Cast<UMAMaterialParamObject>(NewObject<UMAMaterialParamObject>(this, TEXT("MaterialParamObject")))))
	{
		MaterialPropertyView->SetPropertyName(FName(TEXT("NewMaterial")));
		MaterialPropertyView->SetObject(MaterialParamObject);
		if(IsValid(InExistingMaterial))
		{
			MaterialParamObject->SetMaterialParam(InExistingMaterial);
		}
		MaterialParamObject->OnMaterialParamChanged.BindUObject(this, &UMAAssignmentSlot::OnMaterialParamChanged);
	}

	if(InSuggestedMaterial.IsValid())
	{
		if(IsValid(InExistingMaterial))
		{
			if(!InSuggestedMaterial.AssetName.IsEqual(*InExistingMaterial->GetName()))
			{
				AssignSuggestedMaterial(InSuggestedMaterial);
			}
			else
			{
				if(IsValid(QuickAssignBtn)){QuickAssignBtn->SetVisibility(ESlateVisibility::Hidden);}
			}
		}
		else
		{
			AssignSuggestedMaterial(InSuggestedMaterial);
		}
	}
	else
	{
		if(IsValid(QuickAssignBtn)){QuickAssignBtn->SetVisibility(ESlateVisibility::Hidden);}
	}

	DeleteBtn->SetIsEnabled(!IsSlotInUse);
	
	RefreshTheSMNameList();
}

void UMAAssignmentSlot::ApplyBtnPressed()
{
	if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
	{
		const auto ToolWindowRef = ToolSubsystem->GetMAMainScreen();
		if(ToolWindowRef.IsValid())
		{
			if(const auto IToolWindow = Cast<IMAToolWindowInterface>(ToolWindowRef.Get()))
			{
				if(MaterialParamObject && MaterialParamObject->GetMaterialParam())
				{
					DefaultMaterialName = MaterialParamObject->GetMaterialParam()->GetName();
					IToolWindow->ApplySingleMaterialChange(SlotName,MaterialParamObject->GetMaterialParam());
					IToolWindow->MaterialPropertyChanged(false);
				}
			}
		}
	}
	bMaterialParamChangedOnce = false;
	DisableApplyButton();
}

void UMAAssignmentSlot::QuickAssignBtnPressed()
{
	if(const auto SuggestedMaterialObject = SuggestedMaterialData.GetAsset())
	{
		if(IsValid(MaterialParamObject)){MaterialParamObject->SetMaterialParam(Cast<UMaterialInterface>(SuggestedMaterialObject));}
		OnMaterialParamChanged();
		QuickAssignBtn->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMAAssignmentSlot::DeleteBtnPressed()
{
	if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
	{
		const auto ToolWindowRef = ToolSubsystem->GetMAMainScreen();
		if(ToolWindowRef.IsValid())
		{
			if(const auto IToolWindow = Cast<IMAToolWindowInterface>(ToolWindowRef.Get()))
			{
				IToolWindow->DeleteSlot(SlotName);
			}
		}
	}
	bMaterialParamChangedOnce = false;
	DisableApplyButton();

	
}

void UMAAssignmentSlot::OnSlotNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(CommitMethod == ETextCommit::Type::OnEnter)
	{
		if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
		{
			const auto ToolWindowRef = ToolSubsystem->GetMAMainScreen();
			if(ToolWindowRef.IsValid())
			{
				if(const auto IToolWindow = Cast<IMAToolWindowInterface>(ToolWindowRef.Get()))
				{
					if(!Text.IsEmpty() && !Text.ToString().Equals(SlotName.ToString()))
					{
						if(IToolWindow->RenameMaterialSlot(SlotName,*Text.ToString()))
						{
							MADebug::ShowNotifyInfo(FString(TEXT("Material slots renamed succesfully.")));
							SlotName = *Text.ToString();
							SlotRenameText->SetText(FText::FromName(SlotName));


							//Re find suggest material
							TArray<FAssetData> FoundMaterials;
							UMAFunctions::GetAllMaterialInstances(FoundMaterials);
							SuggestedMaterialData = UMAFunctions::FindTheMostSuitableMaterialForTheNameSlot(SlotName.ToString(),FoundMaterials);
							AssignSuggestedMaterial(SuggestedMaterialData);
						}
					}
				}
			}
		}
	}
	SlotRenameText->SetText(FText::FromName(SlotName));
}

void UMAAssignmentSlot::RefreshTheSMNameList()
{
	if(MeshNames.Num() == 0){return;}

	FString LocalString;

	const int32 Num = MeshNames.Num();
	for(int32 Index = 0 ; Index < Num ; Index++)
	{
		LocalString.Append(MeshNames[Index]);
		if(Index == Num - 1){break;}
		LocalString.Append(TEXT("\n"));
	}
	MeshNameList->SetText(FText::FromString(LocalString));
}

void UMAAssignmentSlot::AssignSuggestedMaterial(const FAssetData& InAssetData)
{
	if(InAssetData.IsValid())
	{
		QuickAssignText->SetText(FText::FromName(InAssetData.AssetName));
		QuickAssignText->SetToolTipText(FText::FromName(InAssetData.AssetName));
		SuggestedMaterialData = InAssetData;
		QuickAssignBtn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		QuickAssignBtn->SetVisibility(ESlateVisibility::Hidden);
	}

}


void UMAAssignmentSlot::AddNewObjectToCounter(const FString& InMeshName,bool IsSlotInUse)
{
	MeshNames.Add(InMeshName);
	CounterText->SetText(FText::FromString(FString::FromInt(MeshNames.Num())));
	RefreshTheSMNameList();

	if(IsSlotInUse)
	{
		DeleteBtn->SetIsEnabled(false);
	}
}

void UMAAssignmentSlot::CheckForMaterialDifferences(const UMaterialInterface* InMaterial) const
{
	if(InMaterial && InMaterial->GetName().Equals(DefaultMaterialName))
	{
		return;
	}
	
	MultipleChoiceBox->SetVisibility(ESlateVisibility::Visible);
}

TObjectPtr<UMaterialInterface> UMAAssignmentSlot::GetNewMaterial() const
{
	if(MaterialParamObject){return MaterialParamObject->GetMaterialParam();}
	return nullptr;
}

void UMAAssignmentSlot::DisableApplyButton()
{
	ApplyBtn->SetIsEnabled(false);
}

void UMAAssignmentSlot::AllMaterialsApplied()
{
	DefaultMaterialName = MaterialParamObject->GetMaterialParam()->GetName();
	bMaterialParamChangedOnce = false;
	DisableApplyButton();
}

void UMAAssignmentSlot::OnMaterialParamChanged()
{
	if(!IsValid(MaterialParamObject)){return;}


	if(!IsValid(MaterialParamObject->GetMaterialParam()))
	{
		DisableApplyButton();
		ApplyBtn->SetIsEnabled(false);
		bMaterialParamChangedOnce = false;
		return;
	}
	
	//If returned to default one then disable apply button
	if(DefaultMaterialName.Equals(MaterialParamObject->GetMaterialParam()->GetName()))
	{
		DisableApplyButton();
			
		if(bMaterialParamChangedOnce)
		{
			if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
			{
				const auto ToolWindowRef = ToolSubsystem->GetMAMainScreen();
				if(ToolWindowRef.IsValid())
				{
					if(const auto IToolWindow = Cast<IMAToolWindowInterface>(ToolWindowRef.Get()))
					{
						IToolWindow->MaterialPropertyChanged(false);
						bMaterialParamChangedOnce = false;
					}
				}
			}
		}
		return;
	}
	
	//Notify if did not before
	if(!bMaterialParamChangedOnce)
	{
		if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
		{
			const auto ToolWindowRef = ToolSubsystem->GetMAMainScreen();
			if(ToolWindowRef.IsValid())
			{
				if(const auto IToolWindow = Cast<IMAToolWindowInterface>(ToolWindowRef.Get()))
				{
					IToolWindow->MaterialPropertyChanged(true);
					bMaterialParamChangedOnce = true;
					ApplyBtn->SetIsEnabled(true);
				}
			}
		}
	}
}

void UMAAssignmentSlot::NativeDestruct()
{
	if(IsValid(ApplyBtn)){ApplyBtn->OnClicked.RemoveAll(this);}
	if(IsValid(QuickAssignBtn)){QuickAssignBtn->OnClicked.RemoveAll(this);}
	if(IsValid(SlotRenameText)){SlotRenameText->OnTextCommitted.RemoveAll(this);}
	Super::NativeDestruct();
}
