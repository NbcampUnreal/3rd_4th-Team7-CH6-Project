// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBMaterialAssignmentSection.h"
#include "Editor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Development/MBDebug.h"
#include "UI/MBMaterialAssignmentSlot.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "MBToolAssetData.h"
#include "Materials/MaterialInstance.h"
#include "ThumbnailRendering/ThumbnailManager.h"


//Find Alternative Materials And Create The Slots
bool UMBMaterialAssignmentSection::SetupMaterialAssignmentSlots(UMaterialInstance* InMaterial,const int32 InSlotIndex) const
{
	if (!GEditor || !InMaterial){return false;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return false;}

	SlotNameText->SetText(FText::FromString((FString::Printf(TEXT("Slot %d"),InSlotIndex +1))));
	
	int32 Index;
	auto FoundAssetPath= InMaterial->GetPathName();
	if (FoundAssetPath.FindLastChar('/', Index))
	{
		FoundAssetPath = FoundAssetPath.LeftChop(FoundAssetPath.Len() - Index); 
	}
	
	FString AssetName = InMaterial->GetName();
	int32 index;
	if (AssetName.FindLastChar('_', index))
	{
		AssetName = AssetName.LeftChop(AssetName.Len() - index);
	}
	
	if(CheckForEligibility(AssetName))
	{
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		TArray<FString> PathsToScan;
		PathsToScan.Add(*FoundAssetPath); 
		AssetRegistry.ScanPathsSynchronous(PathsToScan);

		TArray<FAssetData> FoundAssets;
		AssetRegistry.GetAssetsByPath(FName(FoundAssetPath), FoundAssets);
	
		if(FoundAssets.Num() == 0 ){return false;}

		TArray<FAssetData> FilteredAssets;
		for(auto FoundAsset : FoundAssets)
		{
			if(FoundAsset.AssetName.ToString().Contains(AssetName))
			{
				FilteredAssets.Add(FoundAsset);
			}
		}

		if(FilteredAssets.Num() == 0 ){return false;}

		
		for(auto FoundRelatedAsset : FilteredAssets)
		{
			//Create Material Slots
			const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::MaterialAssignmentSlotPath);
			if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
			{
				if (const auto CreatedSubSettingsWidget = Cast<UMBMaterialAssignmentSlot>(CreateWidget(EditorWorld,ClassRef)))
				{
					CreatedSubSettingsWidget->SetSlotParams(FoundRelatedAsset,InSlotIndex);
					MaterialSlotContainer->AddChildToWrapBox(CreatedSubSettingsWidget);
				}
			}
		}
		return true;
	}
	else
	{
		//If Not Eligible Then Add Only Material Itself
		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::MaterialAssignmentSlotPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (const auto CreatedSubSettingsWidget = Cast<UMBMaterialAssignmentSlot>(CreateWidget(EditorWorld,ClassRef)))
			{
				CreatedSubSettingsWidget->SetSlotParams(InMaterial,InSlotIndex);
				MaterialSlotContainer->AddChildToWrapBox(CreatedSubSettingsWidget);
			}
		}
		return true;
	}
	
}

bool UMBMaterialAssignmentSection::CheckForEligibility(const FString& InCroppedAssetName)
{
	if(!InCroppedAssetName.StartsWith("MI_")){return false;}
	
	int32 Count = 0;

	for (int32 i = 0; i < InCroppedAssetName.Len(); i++)
	{
		if (InCroppedAssetName[i] == '_')
		{
			Count++;
		}
	}
	if(Count > 0)
	{
		return true;
	}
	return false;
}

void UMBMaterialAssignmentSection::NativeDestruct()
{
	Super::NativeDestruct();
}
