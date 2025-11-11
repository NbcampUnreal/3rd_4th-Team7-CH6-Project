// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBMaterialAssignmentSlot.h"
#include "LevelEditorViewport.h"
#include "ObjectTools.h"
#include "Components/Button.h"
#include "MBAssetFunctions.h"
#include "Components/Image.h"
#include "MBToolSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Texture2D.h"
#include "Interfaces/MBBuildingManagerInterface.h"
#include "Interfaces/MBMainScreenInterface.h"
#include "Libraries/MBActorFunctions.h"
#include "Materials/MaterialInstance.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/EditorActorSubsystem.h"

void UMBMaterialAssignmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (!MaterialSelectionBtn->OnClicked.IsBound())
	{
		MaterialSelectionBtn->OnClicked.AddDynamic(this, &UMBMaterialAssignmentSlot::MaterialSelectionBtnPressed);
	}
}

void UMBMaterialAssignmentSlot::NativeDestruct()
{
	if(MaterialSelectionBtn){MaterialSelectionBtn->OnClicked.RemoveAll(this);}
	
	Super::NativeDestruct();
}

void UMBMaterialAssignmentSlot::MaterialSelectionBtnPressed() 
{
	if (!GEditor || !MaterialInterface || SlotIndex < 0){return;}
	const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	const UMBToolSubsystem* ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!EditorWorld || !ToolSettingsSubsystem){return;}
	
	if(ToolSettingsSubsystem->bIsDuplicationInprogress)
	{
		if(ToolSettingsSubsystem->GetToolMainScreen().Get())
		{
			if(const auto IMainScreen = Cast<IMBMainScreenInterface>(ToolSettingsSubsystem->GetToolMainScreen().Get()))
			{
				if(const auto IBuildingManager = Cast<IMBBuildingManagerInterface>(IMainScreen->GetBuildingManager()))
				{
					IBuildingManager->ApplyMaterialToDuplicatedActors(SlotIndex,MaterialInterface);
				}
			}
		}
		return;
	}

	//Get First Selected Actor
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	auto SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	UMBActorFunctions::FilterModularActors(SelectedActors);

	if(!SelectedActors.IsEmpty())
	{
		const FText TransactionDescription = FText::FromString(TEXT("Modular actor material changes"));
		GEngine->BeginTransaction(TEXT("MaterialAssignment"), TransactionDescription, nullptr);
		
		for(const auto SelectedActor : SelectedActors)
		{
			SelectedActor->Modify(); //Transact Objects
			
			if(const auto FoundMesh = Cast<AStaticMeshActor>(SelectedActor)->GetStaticMeshComponent())
			{
				FoundMesh->SetMaterial(SlotIndex,MaterialInterface);
			}
		}
		
		GEngine->EndTransaction();
	}
}

void UMBMaterialAssignmentSlot::SetSlotParams(const FAssetData& AssetData,const int32 InSlotIndex)
{
	if(!AssetData.GetAsset()){return;}
	
	SlotIndex = InSlotIndex;
	MaterialInterface = Cast<UMaterialInterface>(AssetData.GetAsset());
	
	if(const auto GeneratedTexture = UMBAssetFunctions::GenerateThumbnailForAsset(AssetData,EMBAssetThumbnailFormant::JPEG))
	{
		MaterialImage->SetBrushFromTexture(GeneratedTexture);
	}
	MaterialSelectionBtn->SetToolTipText(FText::FromName(AssetData.AssetName));
}
