// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODToolSubsystem.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "Engine/DataTable.h"
#include "EditorUtilityLibrary.h"
#include "EngineUtils.h"
#include "IContentBrowserSingleton.h"
#include "ODPresetData.h"
#include "Editor/UnrealEdEngine.h"
#include "ODBoundsVisualizer.h"
#include "ODBoundsVisualizerComponent.h"
#include "ODSettingsObject.h"
#include "ODToolAssetData.h"
#include "ODToolWindow.h"
#include "UnrealEdGlobals.h"
#include "Components/DetailsView.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ODToolSettings.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"

void UODToolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EDITOR
	if (!GEditor) {return;}
	
	LoadDataAssets();
	
	//Register OD Bounds Visualizer
	if(GUnrealEd)
	{
		const TSharedPtr<FODBoundsVisualizer> BoundsVisualizer = MakeShareable(new FODBoundsVisualizer());
		GUnrealEd->RegisterComponentVisualizer(UODBoundsVisualizerComponent::StaticClass()->GetFName(), BoundsVisualizer);
		BoundsVisualizer->OnRegister();
	}
	
	if((ODToolSettings = NewObject<UODToolSettings>(this, FName(TEXT("ODToolSettings")))))
	{
		ODToolSettings->LoadConfig();
	}

	UpdateMixerCheckListAndResetAllCheckStatus();

#endif // WITH_EDITOR
}

void UODToolSubsystem::LoadDataAssets()
{
#if WITH_EDITOR

	if(UObject* ODPresetDataObject = ODToolAssetData::PresetDataPath.TryLoad())
	{
		PresetData = Cast<UDataTable>(ODPresetDataObject);
		
		if(PresetData)
		{
			RegeneratePresetColorMap();
		}
		else
		{
			UE_LOG(LogTemp,Error,TEXT("PresetData could not be loaded properly!"));
		}
	}
	
#endif // WITH_EDITOR
}


void UODToolSubsystem::Deinitialize()
{
	CreatedDistObjects.Empty();
	ObjectDataMap.Empty();
	ObjectDistributionData.Empty();
	
	if(GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UODBoundsVisualizerComponent::StaticClass()->GetFName());
	}
	
	Super::Deinitialize();
}

void UODToolSubsystem::ToolWindowClosed()
{
	//Clean Unfinished objects
	const int32 Num = CreatedDistObjects.Num();
	if(Num != 0)
	{
		for(int32 Index = 0 ; Index < Num ; ++Index)
		{
			if(IsValid(CreatedDistObjects[Num - Index - 1]))
			{
				auto LocalRef = CreatedDistObjects[Num - Index - 1];
				CreatedDistObjects.Remove(LocalRef);
				LocalRef->Destroy();
			}
		}
		CreatedDistObjects.Empty();
	}

	//Clean pop-up menus
	if(ToolSettingWindow.IsValid())
	{
		FSlateApplication::Get().RequestDestroyWindow(ToolSettingWindow.ToSharedRef());
		SettingsObject = nullptr;
		ToolSettingWindow.Reset();
	}

	bIsSimulationInProgress = false;
	bIsSimulating = false;
}

#define LOCTEXT_NAMESPACE "ODToolSubsystem"

void UODToolSubsystem::SpawnSettingsMenu()
{
	if(ToolSettingWindow.IsValid())
	{
		FSlateApplication::Get().RequestDestroyWindow(ToolSettingWindow.ToSharedRef());
		SettingsObject = nullptr;
		ToolSettingWindow.Reset();
		return;
	}
	
	FSlateApplication& SlateApp = FSlateApplication::Get();
	const FVector2D WindowSize = FVector2D(350.f, 187.f);

	if((SettingsObject = Cast<UODSettingsObject>(NewObject<UODSettingsObject>(this, TEXT("ODSettingsObject")))))
	{
		if(const auto SettingsWidget = NewObject<UDetailsView>(this))
		{
			SettingsObject->SetupSettingsObject();
			ToolSettingWindow = SNew(SWindow)
			.Title(FText::FromName("Advanced Tool Settings"))
			.ClientSize(WindowSize)
			.HasCloseButton(true)
			.IsTopmostWindow(true)
			.SupportsMaximize(false)
			.SupportsMinimize(false)
			.SizingRule(ESizingRule::FixedSize)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)
				
				+SScrollBox::Slot()
				[
					SettingsWidget->TakeWidget()
				]

				+SScrollBox::Slot()
				[
					SNew(SBox)
					.Padding(1.0f)
					.HAlign(HAlign_Fill)
					[
						SNew(SButton)
						.Text(LOCTEXT("ResetTool","Reset Tool Parameters"))
						.HAlign(HAlign_Center)
						.ToolTipText(LOCTEXT("ResetToolTooltip","Reset all tool parameters except presets to defaults."))
						.OnClicked_UObject(this,&UODToolSubsystem::OnResetToolPressed)
					]
				]
			];
			
			ToolSettingWindow->MoveWindowTo(SlateApp.GetCursorPos() + FVector2D(- WindowSize.X / 2,0));
			SettingsWidget->SetObject(SettingsObject);
			SlateApp.AddWindow(ToolSettingWindow.ToSharedRef());

			ToolSettingWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this]( const TSharedRef<SWindow>& InWindow)
			{
				SettingsObject = nullptr;
				ToolSettingWindow.Reset();
			}));
		}
	}
}

FReply UODToolSubsystem::OnResetToolPressed()
{
	ODToolSettings->LoadDefaultSettings();
	SettingsObject->SetupSettingsObject();
	OnToolReset.ExecuteIfBound();
	
	return FReply::Handled();
	
}

void UODToolSubsystem::SettingsMenuParamChanged(FName InParamName)
{
	if(	InParamName == TEXT("bDrawSpawnBounds")||
		InParamName == TEXT("BoundsColor")||
		InParamName == TEXT("X")||
		InParamName == TEXT("Y")||
		InParamName == TEXT("Z"))
	{
		OnVisualizationParamChanged.ExecuteIfBound();
	}
	else
	{
		OnPhysicsParamChanged.ExecuteIfBound();
	}
}

#undef LOCTEXT_NAMESPACE

#pragma region Presets

bool UODToolSubsystem::IsPresetAvailable(const FString& InPResetName) const
{
	if(!PresetData){return false;}	
	
	TArray<FString> PresetNames;
	TArray<FName> RowNames = PresetData->GetRowNames();
	if(RowNames.Num() == 0){return false;}
	
	for (const auto RowName : RowNames)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(RowName, "", true))
		{
			if(FoundRow->PresetName.ToString().ToLower().Equals(InPResetName.ToLower()))
			{
				return true;
			}
		}
	}
	return false;
}

FODPresetData* UODToolSubsystem::GetPresetData(const FName& InPResetName) const
{
	if(!PresetData){return nullptr;}	
	
	TArray<FString> PresetNames;
	TArray<FName> RowNames = PresetData->GetRowNames();
	if(RowNames.Num() == 0){return nullptr;}
	
	for (const auto RowName : RowNames)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(RowName, "", true))
		{
			if(FoundRow->PresetName.ToString().ToLower().Equals(InPResetName.ToString().ToLower()))
			{
				return FoundRow;
			}
		}
	}
	return nullptr;
}
FLinearColor MakeRandomColor()
{
	const uint8 Hue =  static_cast<uint8>(FMath::RandRange(0.f, 360.0f));
	constexpr uint8 Saturation = static_cast<uint8>(255);
	constexpr uint8 Value = static_cast<uint8>(255);

	return FLinearColor::MakeFromHSV8(Hue, Saturation, Value);
}

void UODToolSubsystem::AddNewPreset(const FString& InPResetName)
{
	FODPresetData NewPresetData;
	NewPresetData.PresetName = FName(*InPResetName);
	
	NewPresetData.PresetColor = MakeRandomColor();
	
	const FName RowName = DataTableUtils::MakeValidName(*InPResetName);
	PresetData->AddRow(RowName,NewPresetData);

	SavePresetDataToDisk();

	RegeneratePresetColorMap();
}

bool UODToolSubsystem::CreateNewPresetFromSelectedAssets(const FString& InPResetName) const
{
	const auto LocalSelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	
	FODPresetData NewPresetData;
	NewPresetData.PresetName = FName(*InPResetName);
	
	for(const auto LocalObject : LocalSelectedAssets)
	{
		if(const auto LocalSM = Cast<UStaticMesh>(LocalObject))
		{
			FDistObjectData NewDistObjectData;
			NewDistObjectData.StaticMesh = LocalSM;
			NewDistObjectData.OwnerPreset = FName(*InPResetName);
			NewPresetData.DistObjectData.Add(NewDistObjectData);
		}
	}

	if(NewPresetData.DistObjectData.IsEmpty())
	{
		return false;
	}
	
	const FName RowName = DataTableUtils::MakeValidName(*InPResetName);
	PresetData->AddRow(RowName,NewPresetData);
	SavePresetDataToDisk();
	return true;
}

void UODToolSubsystem::SetActivePreset(const FString& InPResetName)
{
	if(InPResetName.ToLower().Equals("no preset"))
	{
		LastSelectedPreset = NAME_None;
	}
	else
	{
		LastSelectedPreset = *InPResetName;
	}
}

void UODToolSubsystem::LoadActivePreset()
{
	if(GetLastSelectedPreset().IsNone())
	{
		ObjectDistributionData.Empty();
		OnPresetLoaded.Broadcast();
		return;
	}
	
	TArray<FName> ModAssetRows = PresetData->GetRowNames();
	for(const FName ModAssetRow : ModAssetRows)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(ModAssetRow, "", true))
		{
			if(FoundRow->PresetName.IsEqual(LastSelectedPreset))
			{
				ObjectDistributionData = FoundRow->DistObjectData;
				
				OnPresetLoaded.Broadcast();
				break;
			}
		}
	}
}

void UODToolSubsystem::SetLastPresetAsActivePreset()
{
	if(!PresetData->GetRowNames().IsEmpty())
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(PresetData->GetRowNames().Last(), "", true))
		{
			LastSelectedPreset = FoundRow->PresetName;
		}
	}
}

void UODToolSubsystem::RenameCurrentPreset(const FString& InNewName)
{
	TArray<FName> RowNames = PresetData->GetRowNames();
	for (const auto RowName : RowNames)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(RowName, "", true))
		{
			if(FoundRow->PresetName.IsEqual(LastSelectedPreset))
			{
				FoundRow->PresetName = FName(*InNewName);
				LastSelectedPreset = FoundRow->PresetName;
				break;
			}
		}
	}
	SavePresetDataToDisk();
}

bool UODToolSubsystem::SaveCurrentPreset() const
{
	TArray<FName> RowNames = PresetData->GetRowNames();
	for (const auto RowName : RowNames)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(RowName, "", true))
		{
			if(FoundRow->PresetName.IsEqual(LastSelectedPreset))
			{
				FoundRow->DistObjectData = ObjectDistributionData;
				SavePresetDataToDisk();
				return true;
			}
		}
	}
	return false;
}

void UODToolSubsystem::RemoveCurrentPreset()
{
	TArray<FName> RowNames = PresetData->GetRowNames();
	for (const auto RowName : RowNames)
	{ 
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(RowName, "", true))
		{
			if(FoundRow->PresetName.IsEqual(LastSelectedPreset))
			{
				PresetData->RemoveRow(RowName);
				LastSelectedPreset = NAME_None;
				SavePresetDataToDisk();
				RegeneratePresetColorMap();
				return;
			}
		}
	}
}


TArray<FString> UODToolSubsystem::GetPresetNames() const
{
	if(!PresetData){return TArray<FString>();}

	TArray<FString> PresetNames;
	PresetNames.Add(TEXT("No Preset"));
	TArray<FName> RowNames = PresetData->GetRowNames();
	if(RowNames.Num() == 0){return PresetNames;}
	
	for (auto RowName : RowNames)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(FName(RowName), "", true))
		{
			if(!FoundRow->PresetName.IsNone())
			{
				PresetNames.Add(FoundRow->PresetName.ToString());
			}
		}
	}
	return PresetNames;
}

void UODToolSubsystem::SavePresetDataToDisk() const
{
	if(PresetData)
	{
		if(PresetData->MarkPackageDirty())
		{
			PresetData->PostEditChange();
			UEditorAssetLibrary::SaveLoadedAsset(PresetData);
		}
	}
}

void UODToolSubsystem::RefreshThePresetDataTableForInvalidData() const
{
	if(!PresetData){return;}

	TArray<FName> ModAssetRows = PresetData->GetRowNames();

	if(ModAssetRows.Num() == 0){return;}

	bool bChangesMade = false;
	
	for(const FName ModAssetRow : ModAssetRows)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(ModAssetRow, "", true))
		{
			const int32 Length = FoundRow->DistObjectData.Num();
			if(Length == 0){continue;}
			
			for(int32 Index = 0; Index < Length; Index++)
			{
				if(!IsSoftObjectValidToUse(FoundRow->DistObjectData[Length - Index - 1].StaticMesh))
				{
					//TODO Remove empty mesh data
					bChangesMade = true;
				}
			}
		}
	}
	if(bChangesMade)
	{
		SavePresetDataToDisk();
	}
}



void UODToolSubsystem::DestroyKillZActors(const TArray<FName>& InActorNames)
{
	const int32 Num = CreatedDistObjects.Num();
	if(Num == 0){return;}
	
	for(int32 Index = 0 ; Index < Num ; Index++)
	{
		if(InActorNames.Contains(*CreatedDistObjects[Num - Index - 1]->GetName()))
		{
			auto ActorToDelete = CreatedDistObjects[Num - Index - 1];
			CreatedDistObjects.Remove(ActorToDelete);
			ActorToDelete->Destroy();
		}
	}
}



void UODToolSubsystem::RegeneratePresetColorMap()
{
	if(!PresetData){return;}

	TArray<FName> PresetRows = PresetData->GetRowNames();

	if(PresetRows.Num() == 0){return;}
	
	for(const FName ModAssetRow : PresetRows)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(ModAssetRow, "", true))
		{
			PresetColorMap.Add(FoundRow->PresetName,FoundRow->PresetColor);
		}
	}
}

FLinearColor UODToolSubsystem::GetColorOfPreset(const FName& InPresetName)
{
	if(const auto FoundColor = PresetColorMap.Find(InPresetName))
	{
		return *FoundColor;
	}
	return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("000000FF")));
}

void UODToolSubsystem::CreateInstanceFromDistribution(const EODMeshConversionType& InTargetType)
{
	//Get Editor World
	if(!GEditor){return;}
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	//Define Mesh Collection Data
	struct FMeshData {
		TObjectPtr<UStaticMesh> StaticMesh;
		TArray<TObjectPtr<UMaterialInterface>> Materials;
		TArray<FTransform> UniqueTransforms; 
	};

	//Create Initial Variables
	TArray<FMeshData> MeshData;
	TObjectPtr<UStaticMeshComponent> CurrentSMComp;
	

	//Create Lambda For Adding Data To MeshData
  	auto AddStaticMeshToMeshData = [&MeshData, &CurrentSMComp]() -> void
	{
  		FMeshData NewMeshData;

  		//First data so add new one and return
  		if(MeshData.IsEmpty())
  		{
  			NewMeshData.StaticMesh = CurrentSMComp->GetStaticMesh();
  			NewMeshData.Materials = CurrentSMComp->GetMaterials();
  			NewMeshData.UniqueTransforms.Add(CurrentSMComp->GetComponentTransform());
  			MeshData.Add(NewMeshData);
  			return;
  		}

  		//Check all the data to see if there is exactly the same
		for (int Index = 0; Index < MeshData.Num(); ++Index)
		{
			//If they are have save mesh
			if(MeshData[Index].StaticMesh == CurrentSMComp->GetStaticMesh())
			{
				//If They Are Same Materials
				int32 MaterialIndex = 0;
				bool bAreMaterialsSame = true;
				for(auto CurrentMat : MeshData[Index].Materials)
				{
					if(CurrentMat != CurrentSMComp->GetMaterial(MaterialIndex))
					{
						bAreMaterialsSame = false;
						break;
					}
					++MaterialIndex;
				}
				
				//Both are exactly same
				if(bAreMaterialsSame)
				{
					//They are same add a new transform
					MeshData[Index].UniqueTransforms.Add(CurrentSMComp->GetComponentTransform());
					return;
				}
			}
		}
  		
  		//No matching existing data so add new
  		NewMeshData.StaticMesh = CurrentSMComp->GetStaticMesh();
  		NewMeshData.Materials = CurrentSMComp->GetMaterials();
  		NewMeshData.UniqueTransforms.Add(CurrentSMComp->GetComponentTransform());
  		MeshData.Add(NewMeshData);
	};

	FBox BoundingBox = GetAllActorsTransportPointWithExtents(CreatedDistObjects);

	for(const auto CurrentActor : CreatedDistObjects)
	{
		const auto CurrentSMActor = Cast<AStaticMeshActor>(CurrentActor);
		if(!CurrentSMActor || !IsValid(CurrentSMActor)){return;}

		CurrentSMComp = CurrentSMActor->GetStaticMeshComponent();

		AddStaticMeshToMeshData();
	}

	if(MeshData.IsEmpty()){return;}
	
	//Spawn And Get An Actor
	FTransform NewActorTransform;
	FVector AverageLocation = BoundingBox.GetCenter();
	AverageLocation.Z = BoundingBox.GetCenter().Z - BoundingBox.GetExtent().Z;
	NewActorTransform.SetTranslation(AverageLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	const auto SpawnedActor = EditorWorld->SpawnActor<AActor>(AActor::StaticClass(),SpawnParams);
	if(!SpawnedActor){return;}

	USceneComponent* CreatedRootComponent = NewObject<USceneComponent>(SpawnedActor,FName(TEXT("RootComponent")));
	if(!CreatedRootComponent){SpawnedActor->Destroy(); return;}
	CreatedRootComponent->bAutoRegister = true;
	SpawnedActor->SetRootComponent(CreatedRootComponent);
	SpawnedActor->AddInstanceComponent(CreatedRootComponent);
	SpawnedActor->AddOwnedComponent(CreatedRootComponent);
	SpawnedActor->SetActorTransform(NewActorTransform);
	CreatedRootComponent->SetFlags(RF_Transactional);

	const FString NewActorLabel = FString::Printf(TEXT("%s_Instance"),*LastSelectedPreset.ToString());
	SpawnedActor->SetActorLabel(NewActorLabel);
	SpawnedActor->GetRootComponent()->SetMobility(EComponentMobility::Static);

	//Create Components
	if(InTargetType == EODMeshConversionType::Sm)
	{
		for(const auto CurrentMeshData : MeshData)
		{
			if(!IsValid(CurrentMeshData.StaticMesh)){continue;}
			
			for(auto CurrentTransform : CurrentMeshData.UniqueTransforms)
			{
				const FName CompName = MakeUniqueObjectName(EditorWorld, UStaticMeshComponent::StaticClass(), FName(*CurrentMeshData.StaticMesh->GetName()));
				UStaticMeshComponent* CreatedSmComp = NewObject<UStaticMeshComponent>(SpawnedActor,CompName);
				if(!CreatedSmComp){continue;}
				
				CreatedSmComp->SetupAttachment(SpawnedActor->GetRootComponent());
				CreatedSmComp->AttachToComponent(SpawnedActor->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
				SpawnedActor->AddInstanceComponent(CreatedSmComp);
				SpawnedActor->AddOwnedComponent(CreatedSmComp);
				CreatedSmComp->SetStaticMesh(CurrentMeshData.StaticMesh);

				for(int32 MatIndex = 0; MatIndex < CurrentMeshData.Materials.Num() ;  ++MatIndex)
				{
					CreatedSmComp->SetMaterial(MatIndex,CurrentMeshData.Materials[MatIndex]);
				}
				CreatedSmComp->SetWorldTransform(CurrentTransform);
				CreatedSmComp->SetMobility(EComponentMobility::Static);
				CreatedSmComp->SetFlags(RF_Transactional);
			}
		}
	}
	else
	{
		for(const auto CurrentMeshData : MeshData)
		{
			if(!IsValid(CurrentMeshData.StaticMesh)){continue;}
			
			TObjectPtr<UClass> ISmClass = InTargetType == EODMeshConversionType::Ism ? UInstancedStaticMeshComponent::StaticClass() : UHierarchicalInstancedStaticMeshComponent::StaticClass();

			//Create Unique Name
			const FName CompName =  InTargetType == EODMeshConversionType::Ism		?
			MakeUniqueObjectName(EditorWorld, UInstancedStaticMeshComponent::StaticClass(), FName(*CurrentMeshData.StaticMesh->GetName()))
																					:
			MakeUniqueObjectName(EditorWorld, UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName(*CurrentMeshData.StaticMesh->GetName()));
			
			TObjectPtr<UInstancedStaticMeshComponent> CreatedIsmComp = NewObject<UInstancedStaticMeshComponent>(SpawnedActor,ISmClass,CompName);
			if(!CreatedIsmComp){continue;}

			CreatedIsmComp->SetupAttachment(SpawnedActor->GetRootComponent());
			CreatedIsmComp->AttachToComponent(SpawnedActor->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
			SpawnedActor->AddInstanceComponent(CreatedIsmComp);
			SpawnedActor->AddOwnedComponent(CreatedIsmComp);

			CreatedIsmComp->SetStaticMesh(CurrentMeshData.StaticMesh);
			CreatedIsmComp->SetWorldTransform(SpawnedActor->GetActorTransform());
			CreatedIsmComp->SetMobility(EComponentMobility::Static);
			CreatedIsmComp->SetFlags(RF_Transactional);

			for(int32 MatIndex = 0; MatIndex < CurrentMeshData.Materials.Num() ;  ++MatIndex)
			{
				CreatedIsmComp->SetMaterial(MatIndex,CurrentMeshData.Materials[MatIndex]);
			}
			
			for(auto CurrentTransform : CurrentMeshData.UniqueTransforms)
			{
				CreatedIsmComp->AddInstance(CurrentTransform,true);
			}
		}
	}
	
	SpawnedActor->RegisterAllComponents();
	// GEngine->EndTransaction();
	
	//Destroy Placeholders
	int32 ObjectNum = CreatedDistObjects.Num();
	for(int32 Index = 0 ; Index < ObjectNum ; ++Index)
	{
		if(AActor* CurrentActor = CreatedDistObjects[ObjectNum - Index - 1])
		{
			CreatedDistObjects.RemoveAt(ObjectNum - Index - 1);
			CurrentActor->Destroy();
		}
	}

	//Selected Newly Created Collection Actor
	if(UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(SpawnedActor,true);
	}
}

FBox UODToolSubsystem::GetAllActorsTransportPointWithExtents(const TArray<AStaticMeshActor*>& InActors)
{
	if(InActors.IsEmpty()){return FBox();}

	FVector PositiveExtent = FVector();
	FVector NegativeExtent = FVector();
	
	int32 Index = 0;
	for(const auto Actor : InActors)
	{
		if(!IsValid(Actor)){continue;}
		
		FVector ActorOrigin;
		FVector ActorExtent;
		Actor->GetActorBounds(false,ActorOrigin,ActorExtent,false);
		const FVector ActorPositiveExtent = ActorOrigin + ActorExtent;
		const FVector ActorNegativeExtent = ActorOrigin - ActorExtent;
		
		if(Index == 0)
		{
			PositiveExtent = ActorPositiveExtent;
			NegativeExtent = ActorNegativeExtent;
			++Index;
			continue;	
		}
		
		if(ActorPositiveExtent.X > PositiveExtent.X)
		{
			PositiveExtent.X = ActorPositiveExtent.X;
		}
		if(ActorPositiveExtent.Y > PositiveExtent.Y)
		{
			PositiveExtent.Y = ActorPositiveExtent.Y;
		}
		if(ActorPositiveExtent.Z > PositiveExtent.Z)
		{
			PositiveExtent.Z = ActorPositiveExtent.Z;
		}
		if(ActorNegativeExtent.X < NegativeExtent.X)
		{
			NegativeExtent.X = ActorNegativeExtent.X;
		}
		if(ActorNegativeExtent.Y < NegativeExtent.Y)
		{
			NegativeExtent.Y = ActorNegativeExtent.Y;
		}
		if(ActorNegativeExtent.Z < NegativeExtent.Z)
		{
			NegativeExtent.Z = ActorNegativeExtent.Z;
		}
		++Index;
	}
	return FBox(NegativeExtent,PositiveExtent);
	
}

bool UODToolSubsystem::AddAssetsToPalette(const TArrayView<FAssetData>& DroppedAssets)
{
	auto AddToPaletteLambda = [&](const FAssetData& DroppedAsset) {
		FDistObjectData NewDistObjectData = FDistObjectData();
		NewDistObjectData.StaticMesh = FSoftObjectPath(DroppedAsset.GetAsset());

		if(!IsInMixerMode())
		{
			NewDistObjectData.OwnerPreset = LastSelectedPreset;
		}
		
		ObjectDistributionData.Add(NewDistObjectData);
	};

	
	bool bIsDataChanged = false;
	if(ObjectDistributionData.IsEmpty())
	{
		for(auto& DroppedAsset : DroppedAssets)
		{
			AddToPaletteLambda(DroppedAsset);
		}
		bIsDataChanged = true;
	}
	else
	{
		//Check if already include 
		for(auto& DroppedAsset : DroppedAssets)
		{
			bool bIsInclude = false;
			for(auto& CurrentDistData : ObjectDistributionData)
			{
				if(!IsSoftObjectValidToUse(CurrentDistData.StaticMesh)){continue;}
				
				if(DroppedAsset.GetAsset()->GetName().Equals(CurrentDistData.StaticMesh.GetAssetName()))
				{
					bIsInclude = true;
					break;
				}
			}
			
			// If not then add to palette
			if(!bIsInclude)
			{
				AddToPaletteLambda(DroppedAsset);

				bIsDataChanged = true;
			}
		}
	}

	return bIsDataChanged;
}

TArray<int32> UODToolSubsystem::GetIndexesOfPreset(const FName& InPresetName)
{
	if(ObjectDistributionData.IsEmpty()){return {};}

	TArray<int32> CollectedIndexes;

	int32 Index = 0;
	for(auto& CurrentData : ObjectDistributionData)
	{
		if(CurrentData.OwnerPreset.IsEqual(InPresetName))
		{
			CollectedIndexes.Add(Index);
		}
		++Index;
	}
	return CollectedIndexes;
}


void UODToolSubsystem::RemoveSelectedAssetsFromPalette()
{
	if(ObjectDistributionData.IsEmpty() || SelectedPaletteSlotIndexes.IsEmpty())
	{
		UE_LOG(LogTemp,Error,TEXT("Unable to operate with corrupted data"));
		return;
	}

	for(const auto& CurrentIndex : SelectedPaletteSlotIndexes)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentIndex))
		{
			ObjectDistributionData.RemoveAt(CurrentIndex);
		}
	}

	LastSelectIndex = -1;
	SelectedPaletteSlotIndexes.Empty();


	if(IsInMixerMode())
	{
		CheckForSelectedPresetAvailabilityOnPalette();
	}
}

void UODToolSubsystem::CheckForSelectedPresetAvailabilityOnPalette()
{
	if(ObjectDistributionData.IsEmpty())
	{
		for(auto& CurrentPresetMap : PresetMixerMapData)
		{
			CurrentPresetMap.CheckState = false;
		}
		return;
	}

	TArray<FName> AvailablePresets;
	
	for(auto& CurrentObject : ObjectDistributionData)
	{
		AvailablePresets.AddUnique(CurrentObject.OwnerPreset);
	}

	for(auto& CurrentMixerMap : PresetMixerMapData)
	{
		CurrentMixerMap.CheckState = AvailablePresets.Contains(CurrentMixerMap.PresetName);
	}
}

void UODToolSubsystem::SyncPaletteItems(TArray<int32> InItems)
{
	TArray<UObject*> Objects;
	
	for(const auto& CurrentItem : InItems)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentItem))
		{
			Objects.Add(ObjectDistributionData[CurrentItem].StaticMesh.LoadSynchronous());
		}
	}
	
	//Sync Content Browser
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToAssets(Objects);
}

void UODToolSubsystem::OpenPaletteItemOnSMEditor(const int32& InItem)
{
	if(ObjectDistributionData.IsValidIndex(InItem))
	{
		if(IsSoftObjectValidToUse(ObjectDistributionData[InItem].StaticMesh))
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(ObjectDistributionData[InItem].StaticMesh.LoadSynchronous());
		}
	}
}

TArray<FDistObjectData*> UODToolSubsystem::GetSelectedDistObjectData()
{
	if(SelectedPaletteSlotIndexes.IsEmpty()){return {};}

	TArray<FDistObjectData*> SelectedDistObjectData;
	
	for(const auto& CurrentIndex : SelectedPaletteSlotIndexes)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentIndex))
		{
			SelectedDistObjectData.Add(&ObjectDistributionData[CurrentIndex]);
		}
	}
	return SelectedDistObjectData;
}

TArray<FDistObjectData*> UODToolSubsystem::GetDistObjectDataWithIndexes(const TArray<int32>& InIndexes)
{
	if(InIndexes.IsEmpty()){return {};}

	TArray<FDistObjectData*> SelectedDistObjectData;
	
	for(const auto& CurrentIndex : InIndexes)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentIndex))
		{
			SelectedDistObjectData.Add(&ObjectDistributionData[CurrentIndex]);
		}
	}
	return SelectedDistObjectData;
}

void UODToolSubsystem::ActivateItemsWithIndex(bool InActivate, const TArray<int32>& InIndex)
{
	for(auto& CurrentIndex : InIndex)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentIndex))
		{
			ObjectDistributionData[CurrentIndex].ActiveStatus = InActivate;
		}
	}
	OnObjectActivateStatusChanged.ExecuteIfBound(InActivate,InIndex);
}
#pragma endregion Presets

void UODToolSubsystem::RestartDensitySession()
{
	InitialTotal = 0;
	DensityMap.Empty();
	
	for(auto& CurrentSlot : SelectedPaletteSlotIndexes)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentSlot))
		{
			DensityMap.Add(CurrentSlot,ObjectDistributionData[CurrentSlot].DistributionProperties.SpawnCount);
		}
	}
	
	for(const auto& CurrentMap : DensityMap)
	{
		InitialTotal += CurrentMap.Value;
	}
}

void UODToolSubsystem::ChangeSlotObjectDensity(const float& InValue)
{
	int32 CalculatedTotalSpawnCount;
	auto Value = InValue;

	//Calculate Total Amount
	if(Value > 0.5)
	{
		Value -= 0.5;
		Value *= 2;
		CalculatedTotalSpawnCount = FMath::Lerp(InitialTotal,static_cast<float>(InitialTotal) / DensityMap.Num() * 100.0f,Value);
	}
	else
	{
		Value  = Value * 2;
		CalculatedTotalSpawnCount = FMath::Lerp(1,InitialTotal,Value);
	}

	//Distribute Total Count
	
	TMap<int32,int32> GeneratedCounts;
	
	if(CalculatedTotalSpawnCount > 1)
	{
		for(const auto& Current : DensityMap)
		{
			auto CalculatedSlotAmount = FMath::TruncToInt(static_cast<float>(Current.Value) /  static_cast<float>(InitialTotal) * CalculatedTotalSpawnCount);
			
			GeneratedCounts.Add(Current.Key,CalculatedSlotAmount);
		}
	}
	else
	{
		const int32 SelectedOneIndex = FMath::RandRange(0,DensityMap.Num() - 1);
		int32 Index = 0; 
		for(const auto& Current : DensityMap)
		{
			if(SelectedOneIndex != Index)
			{
				GeneratedCounts.Add(Current.Key,0);	
			}
			else
			{
				GeneratedCounts.Add(Current.Key,1);
			}
			++Index;
		}
	}
	
	for(const auto& CurrentData : GeneratedCounts)
	{
		if(ObjectDistributionData.IsValidIndex(CurrentData.Key))
		{
			ObjectDistributionData[CurrentData.Key].DistributionProperties.SpawnCount = CurrentData.Value;
		}
	}

	
	OnLocalDensityChanged.Execute();
}

#pragma region PresetMixer

void UODToolSubsystem::UpdateMixerCheckListAndResetAllCheckStatus()
{
	PresetMixerMapData.Empty();
	
	const auto PresetNames = GetPresetNames();
	if(PresetNames.IsEmpty()){return;}
	
	for(auto& PresetName : PresetNames)
	{
		if(!PresetName.ToLower().Equals(TEXT("no preset")))
		{
			PresetMixerMapData.Add(FPresetMixerMapData(PresetName,false));
		}
	}
}

void UODToolSubsystem::ToggleMixerMode()
{
	bInMixerMode  = !bInMixerMode;

	ObjectDistributionData.Empty();

	if(bInMixerMode)
	{
		UpdateMixerCheckListAndResetAllCheckStatus();
	}
	else
	{
		PresetMixerMapData.Empty();
	}

	OnMixerModeChanged.ExecuteIfBound(bInMixerMode);
}

void UODToolSubsystem::MixerPresetChecked(const FPresetMixerMapData& InMixerMapData)
{
	if(InMixerMapData.CheckState)
	{
		if(const auto FoundPresetData = GetPresetData(InMixerMapData.PresetName))
		{
			ObjectDistributionData.Append(FoundPresetData->DistObjectData);
			
			OnAMixerPresetCheckStatusChanged.ExecuteIfBound(true,InMixerMapData.PresetName);
		}
	}
	else
	{
		const int32 MixDatNum = ObjectDistributionData.Num();
		for(auto Index = 0; Index < MixDatNum ; ++Index)
		{
			if(ObjectDistributionData[MixDatNum - Index - 1].OwnerPreset.IsEqual(InMixerMapData.PresetName,ENameCase::CaseSensitive))
			{
				ObjectDistributionData.RemoveAt(MixDatNum - Index - 1);
			}
		}
		
		OnAMixerPresetCheckStatusChanged.ExecuteIfBound(false,InMixerMapData.PresetName);
	}
}

void UODToolSubsystem::RegeneratePresetMixerDataFromScratch()
{
	ObjectDistributionData.Empty();

	TArray<FName> CheckedPresetNames;
	for(auto& LocalName :PresetMixerMapData)
	{
		if(LocalName.CheckState)
		{
			CheckedPresetNames.Add(LocalName.PresetName);
		}
	}
	if(CheckedPresetNames.IsEmpty()){return;}
	
	TArray<FString> PresetNames;
	TArray<FName> RowNames = PresetData->GetRowNames();
	if(RowNames.Num() == 0){return;}

	for (const auto RowName : RowNames)
	{
		if(const auto FoundRow = PresetData->FindRow<FODPresetData>(RowName, "", true))
		{
			if(CheckedPresetNames.Contains(FoundRow->PresetName))
			{
				ObjectDistributionData.Append(FoundRow->DistObjectData);
			}
		}
	}
}

#pragma endregion PresetMixer


