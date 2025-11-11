// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBMaterialAssignmentWindow.h"
#include "Components/PanelWidget.h"
#include "Editor.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "MBToolAssetData.h"
#include "Components/StaticMeshComponent.h"
#include "Development/MBDebug.h"
#include "Engine/StaticMeshActor.h"
#include "Libraries/MBActorFunctions.h"
#include "Materials/MaterialInstance.h"
#include "UI/MBMaterialAssignmentSection.h"


void UMBMaterialAssignmentWindow::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UMBMaterialAssignmentWindow::RedesignTheMaterialWindow() const
{
	if (!GEditor){return false;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return false;}
	
	if(MaterialMainBox->HasAnyChildren())
	{
		MaterialMainBox->ClearChildren();
	}
	
	//Get First Selected Actor
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();

	//Filter for Only Modular Actors
	UMBActorFunctions::FilterModularActors(SelectedActors);
	if(SelectedActors.IsEmpty())
	{
		return false;
	}
	const auto SelectedActor = SelectedActors[0];

	bool bIsCreatedAnySlot  = false;

	//If Implements Interface
	if(SelectedActor && UMBActorFunctions::IsActorFromTool(SelectedActor))
	{
		auto FoundMaterials = Cast<AStaticMeshActor>(SelectedActor)->GetStaticMeshComponent()->GetStaticMesh()->GetStaticMaterials();
		int32 SlotIndex = 0;
		//Create Section for all found slots
		for(const auto FoundMaterial : FoundMaterials)
		{
			if(FoundMaterial.MaterialInterface)
			{
				if(const auto FoundInstance = Cast<UMaterialInstance>(FoundMaterial.MaterialInterface))
				{
					const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::MaterialAssignmentSectionPath);
					if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
					{
						if (const auto CreatedMatAssignmentSectionMenu = Cast<UMBMaterialAssignmentSection>(CreateWidget(EditorWorld,ClassRef)))
						{
							if(CreatedMatAssignmentSectionMenu->SetupMaterialAssignmentSlots(FoundInstance,SlotIndex))
							{
								MaterialMainBox->AddChild(CreatedMatAssignmentSectionMenu);
								bIsCreatedAnySlot = true;
							}
							else
							{
								CreatedMatAssignmentSectionMenu->Destruct();
							}
						}
					}
				}
			}
			++SlotIndex;
		}
	}

	return bIsCreatedAnySlot;
}

