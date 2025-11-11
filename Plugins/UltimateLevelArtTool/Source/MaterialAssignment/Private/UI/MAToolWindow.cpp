// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MAToolWindow.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "FileHelpers.h"
#include "MAAssignmentSlot.h"
#include "MADebug.h"
#include "MAFunctions.h"
#include "MAToolAssetData.h"
#include "MAToolSubsystem.h"
#include "ScopedTransaction.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

#define LOCTEXT_NAMESPACE "Material Assignment Tool"

void UMAToolWindow::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UMAToolWindow::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ApplyAllBtn) && !ApplyAllBtn->OnClicked.IsBound())
	{
		ApplyAllBtn->OnClicked.AddDynamic(this, &UMAToolWindow::ApplyAllBtnPressed);
	}

	if(IsValid(NoMeshText)){NoMeshText->SetVisibility(ESlateVisibility::Hidden);}

	if(IsValid(ApplyAllBox)){ApplyAllBox->SetVisibility(ESlateVisibility::Hidden);}
	
	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
	{
		ToolSettingsSubsystem->SetMAToolMainScreen(this);
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	ContentBrowserModule.GetOnAssetSelectionChanged().AddUObject(this, &UMAToolWindow::HandleOnAssetSelectionChanged);
	ContentBrowserModule.GetOnAssetPathChanged().AddUObject(this, &UMAToolWindow::HandleOnAssetPathChanged);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	AssetRegistry.OnAssetRemoved().AddUObject(this, &UMAToolWindow::HandleOnAssetRemoved);
	AssetRegistry.OnAssetRenamed().AddUObject(this, &UMAToolWindow::HandleOnAssetRenamed);
	
	UpdateSelectedAssets();
	
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace(UObjectRedirector::StaticClass()->GetClassPathName());
}


void UMAToolWindow::HandleOnAssetSelectionChanged(const TArray<FAssetData>& NewSelectedAssets, bool bIsPrimaryBrowser)
{
	UpdateSelectedAssets();
}

void UMAToolWindow::UpdateSelectedAssets()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	const auto LocalSelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	
	if(LocalSelectedAssets.IsEmpty())
	{
		NoMeshText->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(SelectMeshTextAnim);
		
		ResetAllVariables();
		return;
	}
	
	const TArray<UStaticMesh*> NewAssetList = FilterStaticMeshAssets(LocalSelectedAssets);
	
	if(NewAssetList.IsEmpty())
	{
		PlayAnimation(SelectMeshTextAnim);
		NoMeshText->SetVisibility(ESlateVisibility::Visible);
		
		ResetAllVariables();
		return;
	}

	NoMeshText->SetVisibility(ESlateVisibility::Hidden);

	TArray<FAssetData> FoundMaterialInstances;
	UMAFunctions::GetAllMaterialInstances(FoundMaterialInstances);
	
	//For per selected static mesh
	for(auto CurrentAsset : NewAssetList)
	{
		if(SelectedStaticMeshes.Contains(CurrentAsset)){continue;}

		SelectedStaticMeshes.Add(CurrentAsset);

		//For per material slot in the static mesh

		const TArray<FStaticMaterial>& Materials = CurrentAsset->GetStaticMaterials();
		for(int32 Index = 0; Index < Materials.Num(); Index++)
		{
			//If contains then find and add existing slot
			if(SlotNames.Contains(Materials[Index].MaterialSlotName))
			{
				for(const auto CurrentSlot : MaterialSlotWidgets)
				{
					if(CurrentSlot->GetSlotName().IsEqual(Materials[Index].MaterialSlotName))
					{
						const bool bIsMaterialInUsed = UMAFunctions::IsMaterialInUsed(CurrentAsset,Index);
						CurrentSlot->AddNewObjectToCounter(CurrentAsset->GetName(),bIsMaterialInUsed);

						if(Materials[Index].MaterialInterface)
						{
							CurrentSlot->CheckForMaterialDifferences(Materials[Index].MaterialInterface);
						}
					}
				}
			}
			else // Create New Slot & Add In It
			{
				const TSoftClassPtr<UUserWidget> WidgetClassPtr(MAToolAssetData::MaterialAssignmentSlot);
				if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
				{
					if (UMAAssignmentSlot* CreatedAssignmentSlot = Cast<UMAAssignmentSlot>(CreateWidget(EditorWorld, ClassRef)))
					{
						const bool bIsMaterialInUsed = UMAFunctions::IsMaterialInUsed(CurrentAsset,Index);
						
						CreatedAssignmentSlot->InitializeTheSlot(Materials[Index].MaterialSlotName,Materials[Index].MaterialInterface,CurrentAsset->GetName(),UMAFunctions::FindTheMostSuitableMaterialForTheNameSlot(Materials[Index].MaterialSlotName.ToString(),FoundMaterialInstances),bIsMaterialInUsed);
						MaterialSlotWidgets.Add(CreatedAssignmentSlot);
						SlotBox->AddChild(CreatedAssignmentSlot);
						SlotNames.Add(Materials[Index].MaterialSlotName);
					}
				}
			}
		}
	}

	//Remove static meshes that are not in the new list
	bool bListDecreased = false;
	const int32 Num = SelectedStaticMeshes.Num();

	if(Num > 0)
	{
		for(int32 Index = 0 ; Index < Num ; Index++)
		{
			if (!NewAssetList.Contains(SelectedStaticMeshes[Num - Index - 1]))
			{
				SelectedStaticMeshes.RemoveAt(Num - Index - 1);
				bListDecreased = true;
			}
		}
	}
	if(bListDecreased)
	{
		ResetAllVariables();
		UpdateSelectedAssets();
	}
	RecalculateApplyAllBtnStatus();
}

void UMAToolWindow::ResetAllVariables()
{
	SelectedStaticMeshes.Empty();
	SlotBox->ClearChildren();
	MaterialSlotWidgets.Empty();
	SlotNames.Empty();
	ApplyAllBox->SetVisibility(ESlateVisibility::Collapsed);
}

TArray<UStaticMesh*> UMAToolWindow::FilterStaticMeshAssets(const TArray<UObject*>& Assets)
{
	TArray<UStaticMesh*> FilteredAssets;

	for (UObject* Asset : Assets)
	{
		if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(Asset))
		{
			FilteredAssets.Add(StaticMesh);
		}
	}
	return FilteredAssets;
}
void UMAToolWindow::ApplyAllBtnPressed()
{
	if(MaterialSlotWidgets.IsEmpty()){return;}

	TArray<UPackage*> Packages;
	
	for(const auto MaterialSlot : MaterialSlotWidgets)
	{
		if(MaterialSlot->IsMaterialParamChanged())
		{
			MaterialSlot->AllMaterialsApplied();
		
			auto SlotName = MaterialSlot->GetSlotName();
			auto MaterialToApply = MaterialSlot->GetNewMaterial();
		
			if(!MaterialToApply){continue;}

			for(const auto CurrentMesh : SelectedStaticMeshes)
			{
				if(!CurrentMesh){continue;}
			
				bool bIsMeshChanged = false;
			
				auto& MeshStaticMaterials = CurrentMesh->GetStaticMaterials();
			
				for(int32 Index = 0 ; Index < MeshStaticMaterials.Num() ; Index++)
				{
					if(MeshStaticMaterials[Index].MaterialSlotName.IsEqual(SlotName))
					{
						CurrentMesh->Modify();
						MeshStaticMaterials[Index].MaterialInterface = MaterialToApply;
						bIsMeshChanged = true;
						break;
					}
				}
				if(bIsMeshChanged)
				{
					CurrentMesh->PostEditChange();
					Packages.Add(CurrentMesh->GetPackage());
				}
			}
		}
	}
	if(Packages.Num() > 0)
	{
		SaveModifiedAssets(Packages);
	}
	
	ApplyAllBox->SetVisibility(ESlateVisibility::Collapsed);
	AlteredSlotCounter = false;
}

bool IsInEditorAndNotPlaying()
{
	if (!IsInGameThread())
	{
		UE_LOG(LogTemp, Error, TEXT("You are not on the main thread."));
		return false;
	}
	if (!GIsEditor)
	{
		UE_LOG(LogTemp, Error, TEXT("You are not in the Editor."));
		return false;
	}
	if (GEditor->PlayWorld || GIsPlayInEditorWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("The Editor is currently in a play mode."));
		return false;
	}
	return true;
	
}

bool IsAssetRegistryModuleLoading()
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		UE_LOG(LogTemp, Error, TEXT("The AssetRegistry is currently loading."));
		return false;
	}
	return true;
}

bool UMAToolWindow::SaveModifiedAssets(const TArray<UPackage*>& InPackages)
{
	if(!IsInEditorAndNotPlaying() || !IsAssetRegistryModuleLoading()){return false;}
	
	// Save without a prompt
	return UEditorLoadingAndSavingUtils::SavePackages(InPackages, true);
}

void UMAToolWindow::RecalculateApplyAllBtnStatus()
{
	if(MaterialSlotWidgets.IsEmpty())
	{
		ApplyAllBox->SetVisibility(ESlateVisibility::Collapsed);
		AlteredSlotCounter = 0;
		return;
	}

	for(const auto MaterialSlotWidget : MaterialSlotWidgets)
	{
		if(MaterialSlotWidget->IsMaterialParamChanged())
		{
			++AlteredSlotCounter;
		}
	}
	AlteredSlotCounter > 1 ? ApplyAllBox->SetVisibility(ESlateVisibility::Visible) : ApplyAllBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UMAToolWindow::MaterialPropertyChanged(bool bIncreaseCounter)
{
	if(bIncreaseCounter)
	{
		if(++AlteredSlotCounter > 1)
		{
			ApplyAllBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if(--AlteredSlotCounter < 2)
		{
			ApplyAllBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void  UMAToolWindow::ApplySingleMaterialChange(const FName& SlotName, UMaterialInterface* InMaterialInterface)
{
	if(SelectedStaticMeshes.IsEmpty()){return;}

	TArray<UPackage*> Packages;
	for(const auto CurrentSM : SelectedStaticMeshes)
	{
		bool bIsMeshChanged = false;
		
		auto& Materials = CurrentSM->GetStaticMaterials();
		const int32 SlotNum = Materials.Num();
		for(int32 MatIndex = 0 ; MatIndex < SlotNum ; MatIndex++)
		{
			if(Materials[MatIndex].MaterialSlotName.IsEqual(SlotName))
			{
				bIsMeshChanged = true;
				CurrentSM->Modify();
				Materials[MatIndex].MaterialInterface = InMaterialInterface;
				break;
			}
		}
		if(bIsMeshChanged)
		{
			CurrentSM->PostEditChange();
			Packages.Add(CurrentSM->GetPackage());
		}
		
	}
	if(Packages.Num() > 0)
	{
		SaveModifiedAssets(Packages);
	}
}

bool UMAToolWindow::RenameMaterialSlot(const FName& CurrentSlotName, const FName& NewSlotName)
{
	if(SelectedStaticMeshes.IsEmpty()){return false;}
	
	const auto SlotIndex = SlotNames.Find(CurrentSlotName);
	if(SlotIndex >= 0)
	{
		SlotNames[SlotIndex] = NewSlotName;
	}
	else
	{
		return false;
	}

	bool bSlotChanged = false;
	for(const auto CurrentSM : SelectedStaticMeshes)
	{
		auto& Materials = CurrentSM->GetStaticMaterials();
		const int32 SlotNum = Materials.Num();
		for(int32 MatIndex = 0 ; MatIndex < SlotNum ; MatIndex++)
		{
			if(Materials[MatIndex].MaterialSlotName.IsEqual(CurrentSlotName))
			{
				Materials[MatIndex].MaterialSlotName = NewSlotName;
				UEditorAssetLibrary::SaveLoadedAsset(CurrentSM,false);
				bSlotChanged = true;
				break;
			}
		}
	}

	return bSlotChanged;
}

void UMAToolWindow::DeleteSlot(const FName& InSlotName)
{
	const FScopedTransaction Transaction(LOCTEXT("MaterialAssignment_DeleteSlot", "Mateiral Slot Deletion"));

	if(SelectedStaticMeshes.IsEmpty()){return;}
	
	for(const auto CurrentSM : SelectedStaticMeshes)
	{
		auto& Materials = CurrentSM->GetStaticMaterials();

		for (int32 j = Materials.Num() - 1; j >= 0; j--)
		{
			if(Materials[j].MaterialSlotName.IsEqual(InSlotName))
			{
				CurrentSM->Modify();

				Materials.RemoveAt(j);
				
				CurrentSM->PostEditChange();
				break;
			}
		}
	}
	
	FixUpTheList();
}

void UMAToolWindow::HandleOnAssetPathChanged(const FString& NewPath)
{
	FixUpTheList();
}

void UMAToolWindow::HandleOnAssetRemoved(const FAssetData& InAsset)
{
	FixUpTheList();
}

void UMAToolWindow::HandleOnAssetRenamed(const FAssetData& InAsset, const FString& InOldObjectPath)
{
	FixUpTheList();
}



void UMAToolWindow::FixUpTheList()
{
	if(const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMAToolSubsystem>())
	{
		ToolSettingsSubsystem->SetMAToolMainScreen(this);
	}
	if(!SelectedStaticMeshes.IsEmpty())
	{
		ResetAllVariables();
		UpdateSelectedAssets();
	}
}

void UMAToolWindow::NativeDestruct()
{
	if(IsValid(ApplyAllBtn)){ApplyAllBtn->OnClicked.RemoveAll(this);}

	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry"))) {
		const FAssetRegistryModule& assetRegistryModule = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		if (assetRegistryModule.GetPtr()) {
			IAssetRegistry& AssetRegistry = assetRegistryModule.Get();
			AssetRegistry.OnAssetRemoved().RemoveAll(this);
			AssetRegistry.OnAssetRenamed().RemoveAll(this);
		}
	}
	FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	ContentBrowserModule.GetOnAssetSelectionChanged().RemoveAll(this);
	ContentBrowserModule.GetOnAssetPathChanged().RemoveAll(this);
	
	Super::NativeDestruct();
}

void UMAToolWindow::FixUpDirectories() const
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	TArray<FAssetData> FoundAssets;
	AssetRegistryModule.Get().GetAssets(Filter,FoundAssets);

	TArray<UObjectRedirector*> DirectoriesToFixArray;
	for(const FAssetData& AssetData: FoundAssets)
	{
		if(UObjectRedirector* DirectoryToFix = Cast<UObjectRedirector>(AssetData.GetAsset()))
		{
			DirectoriesToFixArray.Add(DirectoryToFix);
		}
	}
	AssetToolsModule.Get().FixupReferencers(DirectoriesToFixArray);
}

#undef LOCTEXT_NAMESPACE