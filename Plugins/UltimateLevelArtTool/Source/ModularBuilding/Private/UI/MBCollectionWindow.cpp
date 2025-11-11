// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBCollectionWindow.h"
#include "UI/MBModActorSlot.h"
#include "Editor.h"
#include "HairStrandsInterface.h"
#include "LevelEditor.h"
#include "MBToolData.h"
#include "UnrealEdGlobals.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/SinglePropertyView.h"
#include "MBToolAssetData.h"
#include "MBToolSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Development/MBDebug.h"
#include "Editor/GroupActor.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/StaticMeshActor.h"
#include "Libraries/MBActorFunctions.h"
#include "Libraries/MBAssetFunctions.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "UI/MBMaterialAssignmentWindow.h"


void UMBCollectionWindow::NativePreConstruct()
{
	Super::NativePreConstruct();

	SingleBatchConversionView->SetObject(this);
	SingleBatchConversionView->SetPropertyName(TEXT("TargetType"));
}

void UMBCollectionWindow::NativeConstruct()
{
	Super::NativeConstruct();

	if(!GEditor){return;}
	
	ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();

	if(!ToolSettingsSubsystem){return;}

	CollectionTitleText->SetText(FText::FromName(ToolSettingsSubsystem->GetToolData()->Collections[ToolSettingsSubsystem->LastActiveCollectionIndex]));
	
	//Actor Selection Tracing
	FLevelEditorModule& levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	levelEditor.OnActorSelectionChanged().AddUObject(this, &UMBCollectionWindow::HandleOnActorSelectionChanged);

	GEditor->OnLevelActorDeleted().AddUObject(this, &UMBCollectionWindow::HandleOnLevelActorDeletedNative);

	if (!SelectAllBtn->OnClicked.IsBound())
	{
		SelectAllBtn->OnClicked.AddDynamic(this, &UMBCollectionWindow::SelectAllBtnPressed);
	}
	if (!SelectCollectionTransportBtn->OnClicked.IsBound())
	{
		SelectCollectionTransportBtn->OnClicked.AddDynamic(this, &UMBCollectionWindow::SelectCollectionTransportBtnPressed);
	}
	if (!CreateBlueprintBtn->OnClicked.IsBound())
	{
		CreateBlueprintBtn->OnClicked.AddDynamic(this, &UMBCollectionWindow::CreateBlueprintBtnPressed);
	}
	if (!MergeCollectionBtn->OnClicked.IsBound())
	{
		MergeCollectionBtn->OnClicked.AddDynamic(this, &UMBCollectionWindow::MergeCollectionBtnPressed);
	}

	if (!SingleConvertToBtn->OnClicked.IsBound())
	{
		SingleConvertToBtn->OnClicked.AddDynamic(this, &UMBCollectionWindow::SingleConvertToBtnPressed);
	}
	if (!DeleteCollectionBtn->OnClicked.IsBound())
	{
		DeleteCollectionBtn->OnClicked.AddDynamic(this, &UMBCollectionWindow::DeleteCollectionBtnPressed);
	}
	if(!CollectionTitleText->OnTextCommitted.IsBound())
	{
		CollectionTitleText->OnTextCommitted.AddDynamic(this,&UMBCollectionWindow::OnTitleTextBoxCommitted);
	}

	if(SelectCollectionTransportBtn){SelectCollectionTransportBtn->SetToolTipText(FText::FromName(TEXT("Selects the transport point that belongs to the collection.")));}
	if(SelectAllBtn){SelectAllBtn->SetToolTipText(FText::FromName(TEXT("Selects all modular actors that are part of the collection.")));}
	if(SingleConvertToBtn){SingleConvertToBtn->SetToolTipText(FText::FromName(TEXT("Creates an instance where all static meshes in the collection are batched under a single actor, based on the selected type.")));}
	if(CreateBlueprintBtn){CreateBlueprintBtn->SetToolTipText(FText::FromName(TEXT("Creates a blueprint from all actors that are part of the collection.")));}
	if(MergeCollectionBtn){MergeCollectionBtn->SetToolTipText(FText::FromName(TEXT("Runs the built-in merge tool for all actors that are part of the collection.")));}
	if(MergeCollectionBtn){MergeCollectionBtn->SetToolTipText(FText::FromName(TEXT("Deletes the collection and destroys all actors that are part of the collection.")));}
	
	RegenerateTheSettingsMenu();
	RegenerateTypeSelectionTab();
	
	if(MaterialAssignmentBox){MaterialAssignmentBox->SetVisibility(ESlateVisibility::Collapsed);}

	SelectCollectionTransportBtnPressed();

	if(GUnrealEd && TransportPoint)
	{
		GUnrealEd->Exec(TransportPoint->GetWorld(), TEXT("CAMERA ALIGN ACTIVEVIEWPORTONLY"));
	}
}


void UMBCollectionWindow::RegenerateTypeSelectionTab()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}

	ModularSlotBox->ClearChildren();
	PropSlotBox->ClearChildren();
	ModularStaticMeshes.Empty();
	PropStaticMeshes.Empty();

	//MODULAR
	TArray<AActor*> ModularCategoryActors = GetModularCategoryActors();
	if(ModularCategoryActors.IsEmpty()){return;}
	
	UMBActorFunctions::FilterForCategory(EBuildingCategory::Modular,ModularCategoryActors);
	
	if(!ModularCategoryActors.IsEmpty())
	{
		for(const auto CollectionActor : ModularCategoryActors)
		{
			auto StaticMesh = Cast<AStaticMeshActor>(CollectionActor)->GetStaticMeshComponent()->GetStaticMesh();
			ModularStaticMeshes.AddUnique(StaticMesh);
		}
		if(ModularStaticMeshes.Num() > 0)
		{
			ModularTabBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			int32 ModSlotIndex = 0;
			for(const auto ModularStaticMesh : ModularStaticMeshes)
			{
				FAssetData AssetData = UMBAssetFunctions::GetAssetDataFromObject(ModularStaticMesh);

				const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ModActorSlotPath);
				if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
				{
					if (UMBModActorSlot* TabActorSlot = Cast<UMBModActorSlot>(CreateWidget(EditorWorld, ClassRef)))
					{
						TabActorSlot->SetSlotParams(AssetData,ModSlotIndex++,0);
						TabActorSlot->OnModActorSlotClickedSignature.BindDynamic(this,&UMBCollectionWindow::OnActorSlotPressed);
						ModularSlotBox->AddChild(TabActorSlot);
					}
				}
			}
		}
	}
	else
	{
		ModularTabBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	//PROP
	TArray<AActor*>PropCategoryActors = GetPropCategoryActors();
	UMBActorFunctions::FilterForCategory(EBuildingCategory::Prop,PropCategoryActors);
	if(!PropCategoryActors.IsEmpty())
	{
		for(const auto CollectionActor : PropCategoryActors)
		{
			auto StaticMesh = Cast<AStaticMeshActor>(CollectionActor)->GetStaticMeshComponent()->GetStaticMesh();
			PropStaticMeshes.AddUnique(StaticMesh);
		}
		if(PropStaticMeshes.Num() > 0)
		{
			PropTabBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if(PropCategoryActors.IsEmpty()){return;}
			//
			for(const auto CollectionActor : PropCategoryActors)
			{
				auto StaticMesh = Cast<AStaticMeshActor>(CollectionActor)->GetStaticMeshComponent()->GetStaticMesh();
				PropStaticMeshes.AddUnique(StaticMesh);
			}
			int32 PropSlotIndex = 0;
			for(const auto PropStaticMesh : PropStaticMeshes)
			{
				FAssetData AssetData = UMBAssetFunctions::GetAssetDataFromObject(PropStaticMesh);

				const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ModActorSlotPath);
				if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
				{
					if (UMBModActorSlot* TabActorSlot = Cast<UMBModActorSlot>(CreateWidget(EditorWorld, ClassRef)))
					{
						TabActorSlot->SetSlotParams(AssetData,PropSlotIndex++,1);
						TabActorSlot->OnModActorSlotClickedSignature.BindDynamic(this,&UMBCollectionWindow::OnActorSlotPressed);
						PropSlotBox->AddChild(TabActorSlot);
					}
				}
			}
		}
	}
	else
	{
		PropTabBox->SetVisibility(ESlateVisibility::Hidden);
	}
	
	RegenerateTheSettingsMenu();
}

void UMBCollectionWindow::SelectAllBtnPressed()
{
	//Get All Collection Actors

	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(ToolSettingsSubsystem->GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return;}
	
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	EditorActorSubsystem->SelectNothing();
	EditorActorSubsystem->SetSelectedLevelActors(CollectionActors);
	
}

void UMBCollectionWindow::SelectCollectionTransportBtnPressed()
{
	if(!TransportPoint)
	{
		TransportPoint = CreateTransportPoint();
	}

	if(TransportPoint)
	{
		UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(TransportPoint,true);
	}
}

void UMBCollectionWindow::OnActorSlotPressed(int32 InIndex, uint8 InSection)
{
	TArray<AActor*> ActorsToSearch;
	UStaticMesh* SelectedMesh;
	
	if(InSection == 0)
	{
		ActorsToSearch = GetModularCategoryActors();
		SelectedMesh = ModularStaticMeshes[InIndex];

	}
	else if(InSection == 1)
	{
		ActorsToSearch = GetPropCategoryActors();
		SelectedMesh = PropStaticMeshes[InIndex];
	}
	

	TArray<AActor*> ActorsToSelect;
	for(const auto LocalActor : ActorsToSearch)
	{
		if(!LocalActor){continue;}
		if(SelectedMesh == Cast<AStaticMeshActor>(LocalActor)->GetStaticMeshComponent()->GetStaticMesh())
		{
			ActorsToSelect.Add(LocalActor);
		}
	}
		
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	EditorActorSubsystem->SelectNothing();
	EditorActorSubsystem->SetSelectedLevelActors(ActorsToSelect);


	if(MaterialAssignmentWindow)
	{
		const bool bIsCreatedAnySlots =  MaterialAssignmentWindow->RedesignTheMaterialWindow();
		MaterialAssignmentWindow->SetVisibility(bIsCreatedAnySlots ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UMBCollectionWindow::CreateBlueprintBtnPressed()
{
	if(ToolSettingsSubsystem)
	{
		ToolSettingsSubsystem->CreateAssetFromCurrentCollection();
	}
}

void UMBCollectionWindow::MergeCollectionBtnPressed()
{
	if(ToolSettingsSubsystem)
	{
		ToolSettingsSubsystem->RunMergeToolForCurrentCollection();
	}
}

void UMBCollectionWindow::SingleConvertToBtnPressed()
{
	
	if(ToolSettingsSubsystem)
	{
		ToolSettingsSubsystem->BatchCurrentCollection(TargetType);
	}
}

void UMBCollectionWindow::DeleteCollectionBtnPressed()
{
	if(ToolSettingsSubsystem)
	{
		ToolSettingsSubsystem->DeleteCurrentCollection();
	}
}

TObjectPtr<AActor> UMBCollectionWindow::CreateTransportPoint()
{
	if(TransportPoint || !ToolSettingsSubsystem){return TransportPoint;}
	
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(ToolSettingsSubsystem->GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return nullptr;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return nullptr;}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return nullptr;}
	TransportPoint = EditorWorld->SpawnActor<AActor>(AStaticMeshActor::StaticClass(),ReCalculateTransportPoint(),FRotator::ZeroRotator,SpawnParams);
	
	TransportPoint->SetFolderPath(CollectionActors[0]->GetFolderPath());
	const FString TransportPointName = FString::Printf(TEXT("%s_TransformPoint"),*ToolSettingsSubsystem->ActiveCollectionWindow.ToString()); 
	TransportPoint->SetActorLabel(TransportPointName,true);
	
	const FAttachmentTransformRules AttachmentTransformRules  = FAttachmentTransformRules::KeepWorldTransform;
	for(const auto CollectionActor : CollectionActors)
	{
		CollectionActor->AttachToActor(TransportPoint,AttachmentTransformRules);
	}
	
	return TransportPoint;
}

FVector UMBCollectionWindow::ReCalculateTransportPoint() const
{
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(ToolSettingsSubsystem->GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return FVector::ZeroVector;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return FVector::ZeroVector;}
	
	const auto CollectionBox = UMBActorFunctions::GetAllActorsTransportPointWithExtents(CollectionActors);
	FVector SpawnLocation = CollectionBox.GetCenter();
	SpawnLocation.Z = CollectionBox.GetCenter().Z - CollectionBox.GetExtent().Z;
	return SpawnLocation;
}

TArray<AActor*> UMBCollectionWindow::GetModularCategoryActors() const
{
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(ToolSettingsSubsystem->GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return TArray<AActor*>();}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return TArray<AActor*>();}
	UMBActorFunctions::FilterForCategory(EBuildingCategory::Modular,CollectionActors);
	return CollectionActors;
}

TArray<AActor*> UMBCollectionWindow::GetPropCategoryActors() const
{
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(ToolSettingsSubsystem->GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return TArray<AActor*>();}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return TArray<AActor*>();}
	UMBActorFunctions::FilterForCategory(EBuildingCategory::Prop,CollectionActors);
	return CollectionActors;
}

void UMBCollectionWindow::HandleOnActorSelectionChanged(const TArray<UObject*>& NewSelection, bool bForceRefresh)
{
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(ToolSettingsSubsystem->GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return;}
	
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty())
	{
		MaterialAssignmentBox->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	bool bContains = true;
	for (const UObject* Object : NewSelection) {

		if(!CollectionActors.Contains(Object))
		{
			if(Object->IsA(AGroupActor::StaticClass()))
			{
				continue;
			}
			bContains = false;
			break;
		}
	}
	if(bContains)
	{
		MaterialAssignmentBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		MaterialAssignmentBox->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UMBCollectionWindow::HandleOnLevelActorDeletedNative(AActor* Actor)
{
	RegenerateTypeSelectionTab();
}

void UMBCollectionWindow::OnTitleTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(!ToolSettingsSubsystem){return;}
	const int32 Index = ToolSettingsSubsystem->LastActiveCollectionIndex;
	if(Index < 0){return;}

	const FName CurrentCollectionName = ToolSettingsSubsystem->GetToolData()->Collections[Index];
	
	if(CommitMethod != ETextCommit::Type::OnEnter || Text.IsEmpty())
	{
		CollectionTitleText->SetText(FText::FromName(CurrentCollectionName));
	}
	else
	{
		ToolSettingsSubsystem->ChangeCollectionName(CurrentCollectionName,FName(*Text.ToString()));
	}
}

void UMBCollectionWindow::RegenerateTheSettingsMenu()
{
	MaterialAssignmentBox->ClearChildren();
	
	//Create Material Slot
	if (!GEditor){return;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}

	const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::MaterialAssignmentWindowPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if ((MaterialAssignmentWindow = Cast<UMBMaterialAssignmentWindow>(CreateWidget(EditorWorld,ClassRef))))
		{
			const bool bIsCreatedAnySlots =  MaterialAssignmentWindow->RedesignTheMaterialWindow();

			MaterialAssignmentWindow->SetVisibility(bIsCreatedAnySlots ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			
			MaterialAssignmentBox->AddChild(MaterialAssignmentWindow);
		}
	}
}

void UMBCollectionWindow::NativeDestruct()
{
	if (SelectAllBtn) { SelectAllBtn->OnClicked.RemoveAll(this); }
	if (SelectCollectionTransportBtn) { SelectCollectionTransportBtn->OnClicked.RemoveAll(this); }
	if (CreateBlueprintBtn) { CreateBlueprintBtn->OnClicked.RemoveAll(this); }
	if (MergeCollectionBtn) { MergeCollectionBtn->OnClicked.RemoveAll(this); }
	if (SingleConvertToBtn) { SingleConvertToBtn->OnClicked.RemoveAll(this); }
	if (DeleteCollectionBtn) { DeleteCollectionBtn->OnClicked.RemoveAll(this); }
	if (CollectionTitleText) { CollectionTitleText->OnTextCommitted.RemoveAll(this); }


	if(GEditor)
	{
		GEditor->OnLevelActorDeleted().AddUObject(this, &UMBCollectionWindow::HandleOnLevelActorDeletedNative);
	}

	FLevelEditorModule& levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	levelEditor.OnActorSelectionChanged().RemoveAll(this);
	
	if(TransportPoint){TransportPoint->Destroy();}
	
	Super::NativeDestruct();
}


