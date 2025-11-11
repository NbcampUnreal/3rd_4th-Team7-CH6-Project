// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "MBToolSubsystem.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "Data/MBModularAssetData.h"
#include "Data/MBToolData.h"
#include "Development/MBDebug.h"
#include "Engine/DataTable.h"
#include "Interfaces/MBMainScreenInterface.h"
#include "Libraries/MBActorFunctions.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EngineUtils.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"
#include "EditorActorFolders.h"
#include "LevelEditor.h"
#include "MBToolAssetData.h"
#include "MBUserSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/StaticMeshActor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "LevelEditorViewport.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

#define SandboxFolderPath "Prefabs/Sandbox"

struct FBlueprintAssemblyPropsTool
{
	FBlueprintAssemblyPropsTool(UBlueprint* InBlueprint, const TArray<AActor*>& InActors)
		: Blueprint(InBlueprint)
		, Actors(InActors)
	{
	}

	UBlueprint* Blueprint; 
	const TArray<AActor*>& Actors;
	TArray<AActor*> RootActors;
	TArray<USCS_Node*>* OutNodes = nullptr;
	TMap<AActor*, TArray<AActor*>> AttachmentMap;
	USCS_Node* RootNodeOverride = nullptr;
};

void UMBToolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EDITOR
	if (!GEditor) {return;}
	
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	AssetRegistry.OnAssetRemoved().AddUObject(this, &UMBToolSubsystem::HandleOnAssetRemoved);
	AssetRegistry.OnAssetRenamed().AddUObject(this, &UMBToolSubsystem::HandleOnAssetRenamed);

	LoadDataAssets();

#endif // WITH_EDITOR

}

void UMBToolSubsystem::LoadDataAssets()
{
#if WITH_EDITOR
	if(UObject* ModularAssetDataObject = MBToolAssetData::ModularAssetDataPath.TryLoad())
	{
		ModularAssetData = Cast<UDataTable>(ModularAssetDataObject);
		
		/*if(ModularAssetData)
		{
			CheckForNullModularDataAndRemove();
		}*/
	}
	if(UObject* ToolUserSettingsObject = MBToolAssetData::ToolUserSettingsDataPath.TryLoad())
	{
		ToolUserSettings = Cast<UMBUserSettings>(ToolUserSettingsObject);
	}
	if(UObject* ToolDataObject = MBToolAssetData::ToolDataPath.TryLoad())
	{
		ToolData = Cast<UMBToolData>(ToolDataObject);
	}

	if(!ModularAssetData || !ToolUserSettings || !ToolData)
	{
		UE_LOG(LogTemp,Error,TEXT("The plugin could not be loaded properly!"));
	}
#endif // WITH_EDITOR
}



void UMBToolSubsystem::Deinitialize()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")))
	{
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		if (AssetRegistryModule.GetPtr()) {
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
			AssetRegistry.OnAssetRemoved().RemoveAll(this);
			AssetRegistry.OnAssetRenamed().RemoveAll(this);
		}
	}
	Super::Deinitialize();
}

void UMBToolSubsystem::SaveAllModularData() const
{
	SaveModularAssetDataToDisk();
	
	SaveMBToolDataToDisk();
}


void UMBToolSubsystem::SaveModularAssetDataToDisk() const
{
	if(ModularAssetData && IsValid(ModularAssetData))
	{
		if(ModularAssetData->MarkPackageDirty())
		{
			ModularAssetData->PostEditChange();
			UEditorAssetLibrary::SaveLoadedAsset(ModularAssetData);
		}
	}
}

void UMBToolSubsystem::SaveMBToolDataToDisk() const
{
	if(ToolData && IsValid(ToolData))
	{
		if(ToolData->MarkPackageDirty())
		{
			ToolData->PostEditChange();
			UEditorAssetLibrary::SaveLoadedAsset(ToolData);
		}
	}
}

void UMBToolSubsystem::CheckForNullModularDataAndRemove() const
{
	if (!ModularAssetData) return;
	TArray<FName> ModAssetRows = ModularAssetData->GetRowNames();
	
	if(ModAssetRows.Num() == 0){return;}
	
	TArray<FName> RowsToRemove;
	
	for (const FName& ModAssetRow : ModAssetRows)
	{
		if (const FModularBuildingAssetData* FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(ModAssetRow, TEXT(""), true))
		{
			const TSoftObjectPtr<UObject>& AssetRef = FoundRow->AssetReference;

			if (AssetRef.IsNull() || !AssetRef.ToSoftObjectPath().ResolveObject())
			{
				RowsToRemove.Add(ModAssetRow);
			}
		}
	}
	
	if(RowsToRemove.Num() > 0)
	{
		for(const FName RowToRemove : RowsToRemove)
		{
			if (ModularAssetData->FindRow<FModularBuildingAssetData>(FName(RowToRemove), "", true))
			{
				ModularAssetData->RemoveRow(RowToRemove);
			}
		}
	}
}

bool UMBToolSubsystem::AddNewMeshType(const FName InTypeName, const EBuildingCategory InCategory) const
{
	if(!ToolData){return false;}
	if(InCategory == EBuildingCategory::Modular)
	{
		if(ToolData->ModularMeshTypes.Contains(InTypeName))
		{
			return false;
		}
		ToolData->ModularMeshTypes.Add(InTypeName);
	}
	else if(InCategory == EBuildingCategory::Prop)
	{
		if(ToolData->PropMeshTypes.Contains(InTypeName))
		{
			return false;
		}
		ToolData->PropMeshTypes.Add(InTypeName);
	}
	SaveMBToolDataToDisk();

	return true;
}

bool UMBToolSubsystem::ChangeAssetType(const FName InOldTypeName,const FName InNewTypeName,const EBuildingCategory InCategory)
{
	if(!ToolData){return false;}
	if(InCategory == EBuildingCategory::Modular)
	{
		if(ToolData->ModularMeshTypes.Contains(InNewTypeName))
		{
			return false;
		}
		ToolData->ModularMeshTypes[ToolData->ModularMeshTypes.Find(InOldTypeName)] = InNewTypeName;
	}
	else if(InCategory == EBuildingCategory::Prop)
	{
		if(ToolData->PropMeshTypes.Contains(InNewTypeName))
		{
			return false;
		}
		ToolData->PropMeshTypes[ToolData->PropMeshTypes.Find(InOldTypeName)] = InNewTypeName;
	}
	
	if (!ModularAssetData) return false;
	TArray<FName> RowNames = ModularAssetData->GetRowNames();
	for (auto RowName : RowNames)
	{
		if(const auto FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(FName(RowName), "", true))
		{
			if(InCategory == FoundRow->MeshCategory)
			{
				if(FoundRow->MeshType.IsEqual(InOldTypeName))
				{
					FoundRow->MeshType = InNewTypeName;
				}
			}
		}
	}
	SaveModularAssetDataToDisk();
	
	if(ToolData)
	{
		SaveMBToolDataToDisk();
	}
	if(MBToolMainScreen.IsValid())
	{
		Cast<IMBMainScreenInterface>(MBToolMainScreen.Get())->UpdateCategoryWindow();
	}
	return true;
}

bool UMBToolSubsystem::RemoveAssetType(const FName InTypeName, const EBuildingCategory InCategory) const
{
	if(!ToolData)
	{
		return false;
	}
	if(InCategory == EBuildingCategory::Modular)
	{
		if(!ToolData->ModularMeshTypes.Contains(InTypeName))
		{
			return false;
		}
		if(RemoveSameTypedAssetsFromTool(InTypeName,InCategory))
		{
			ToolData->ModularMeshTypes.Remove(InTypeName);
		}
	}
	else if(InCategory == EBuildingCategory::Prop)
	{
		if(!ToolData->PropMeshTypes.Contains(InTypeName))
		{
			return false;
		}
		if(RemoveSameTypedAssetsFromTool(InTypeName,InCategory))
		{
			ToolData->PropMeshTypes.Remove(InTypeName);
		}
	}
	
	if(RemoveSameTypedAssetsFromTool(InTypeName,InCategory))
	{
		if(MBToolMainScreen.IsValid())
		{
			Cast<IMBMainScreenInterface>(MBToolMainScreen.Get())->AssetTypeRemoved();
		}
		SaveMBToolDataToDisk();

		return true;
	}
	return false;
}

bool UMBToolSubsystem::ChangeTypeOrder(const FName InTypeName, const EBuildingCategory InCategory,bool bInIsGoingUp) const
{
	if(bInIsGoingUp)
	{
		if(GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
		{
			const int32 TypeIndex = GetToolData()->ModularMeshTypes.Find(InTypeName);
			{
				if(TypeIndex > 0)
				{
					GetToolData()->ModularMeshTypes.Swap(TypeIndex,TypeIndex - 1);
					SaveMBToolDataToDisk();
					return true;
				}
			}
		}
		else if(GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Prop)
		{
			const int32 TypeIndex = GetToolData()->PropMeshTypes.Find(InTypeName);
			{
				if(TypeIndex > 0)
				{
					GetToolData()->PropMeshTypes.Swap(TypeIndex,TypeIndex - 1);
					SaveMBToolDataToDisk();
					return true;
				}
			}
		}
	}
	else
	{
		if(GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
		{
			const int32 TypeIndex = GetToolData()->ModularMeshTypes.Find(InTypeName);
			{
				if(TypeIndex < GetToolData()->ModularMeshTypes.Num() - 1)
				{
					GetToolData()->ModularMeshTypes.Swap(TypeIndex,TypeIndex + 1);
					SaveMBToolDataToDisk();
					return true;
				}
			}
		}
		else if(GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Prop)
		{
			const int32 TypeIndex = GetToolData()->PropMeshTypes.Find(InTypeName);
			{
				if(TypeIndex < GetToolData()->PropMeshTypes.Num() - 1)
				{
					GetToolData()->PropMeshTypes.Swap(TypeIndex,TypeIndex + 1);
					SaveMBToolDataToDisk();
					return true;
				}
			}
		}
	}

	return false;
}

bool UMBToolSubsystem::RemoveSameTypedAssetsFromTool(const FName& InTypeName, const EBuildingCategory& InCategory) const
{
	TArray<FName> RowsToRemove;
	TArray<FName> RowNames = ModularAssetData->GetRowNames();
	for (auto RowName : RowNames)
	{
		if(const auto FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(FName(RowName), "", true))
		{
			if(InCategory == FoundRow->MeshCategory)
			{
				if(FoundRow->MeshType == InTypeName)
				{
					RowsToRemove.Add(RowName);
				}
			}
		}
	}
	if(RowsToRemove.Num() > 0)
	{
		for(const auto RowToRemove : RowsToRemove)
		{
			ModularAssetData->RemoveRow(RowToRemove);
		}
	}
	SaveModularAssetDataToDisk();
	return true;
}

void UMBToolSubsystem::RemoveAssetByName(const FString& InName) const
{
	TArray<FName> RowsToRemove;
	TArray<FName> RowNames = ModularAssetData->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		if (const FModularBuildingAssetData* FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(RowName, TEXT(""), true))
		{
			if (FoundRow->AssetReference.IsValid() && FoundRow->AssetReference->GetName().Equals(InName))
			{
				ModularAssetData->RemoveRow(RowName);
				break;
			}
		}
	}
	SaveModularAssetDataToDisk();
	
	if(MBToolMainScreen.IsValid())
	{
		Cast<IMBMainScreenInterface>(MBToolMainScreen.Get())->UpdateCategoryWindow();
	}

	const FString Dialog = FString::Printf(TEXT("Asset removed succesfully."));
	MBDebug::ShowNotifyInfo(Dialog);
}

int32 UMBToolSubsystem::CreateANewCollection() const
{
	if (!GEditor) return -1;
	const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld) return -1;
	TArray<FName>& Collections = ToolData->Collections;
	Collections.Add(FName(CreteAUniqueCollectionName(Collections)));
	SaveMBToolDataToDisk();
	return Collections.Num() -1;
}

FName UMBToolSubsystem::CreteAUniqueCollectionName(const TArray<FName>& InCollections)
{
	FName NewCollectionName = FName(FString::Printf(TEXT("Collection%d"), InCollections.Num() + 1));
	if(InCollections.IsEmpty())
	{
		return NewCollectionName;
	}
	if(!InCollections.IsEmpty() && !InCollections.Contains(NewCollectionName))
	{
		return NewCollectionName;
	}	
	
	const int32 MaxTry = InCollections.Num() + 100;
	for(uint8 Suffix = InCollections.Num() + 2 ; Suffix < MaxTry ; ++Suffix)
	{
		NewCollectionName = FName(FString::Printf(TEXT("Collection%d"),Suffix));

		if(InCollections.Contains(NewCollectionName))
		{
			continue;
		}
		return NewCollectionName;
	}
	for(uint8 Suffix = InCollections.Num() + 1 ; Suffix < MaxTry ; ++Suffix)
	{
		NewCollectionName = FName(FString::Printf(TEXT("Collection_%d"),Suffix));

		if(!InCollections.Contains(NewCollectionName))
		{
			continue;
		}
		return NewCollectionName;
	}
	for(uint8 Suffix = InCollections.Num() + 1 ; Suffix < MaxTry ; ++Suffix)
	{
		NewCollectionName = FName(FString::Printf(TEXT("Collection_%d_%d"),Suffix,Suffix));

		if(!InCollections.Contains(NewCollectionName))
		{
			continue;
		}
		return NewCollectionName;
	}
	for(uint8 Suffix = InCollections.Num() + 1 ; Suffix < MaxTry ; ++Suffix)
	{
		NewCollectionName = FName(FString::Printf(TEXT("Collection_%d_%d_%d"),Suffix,Suffix,Suffix));

		if(!InCollections.Contains(NewCollectionName))
		{
			continue;
		}
		return NewCollectionName;
	}
	return NewCollectionName;

}

bool UMBToolSubsystem::ChangeCollectionName(const FName InOldName, const FName InNewName) const
{
	if(ToolData->Collections.Contains(InNewName))
	{
		return false;
	}

	if (!GEditor){return false;}
	const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return false;}

	const auto NewFolderPath = FName(*FString::Printf(TEXT("Prefabs/%s"),*InNewName.ToString()));

	TArray<AActor*> ActorsToMove = UMBActorFunctions::GetAllActorsUnderTheFolderPath(GetActiveCollectionFolderPath());
	if(!ActorsToMove.IsEmpty())
	{
		for(const auto Actor : ActorsToMove)
		{
			Actor->SetFolderPath(NewFolderPath);
		}
	}

	const auto Index = GetToolData()->Collections.Find(InOldName);
	if(Index >= 0)
	{
		GetToolData()->Collections[Index] = InNewName;
		SaveMBToolDataToDisk();
	}
	else
	{
		return false;
	}

	if(MBToolMainScreen.IsValid())
	{
		Cast<IMBMainScreenInterface>(MBToolMainScreen.Get())->CollectionNameChanged(InOldName,InNewName);
	}
	
	return true;
}

void RepositionNodes(const TArray<USCS_Node*>& Nodes, const FTransform& Pivot)
{
	for (const USCS_Node* Node : Nodes)
	{
		if (USceneComponent* NodeTemplate = Cast<USceneComponent>(Node->ComponentTemplate))
		{
			//The relative transform for those component was converted into the world space
			if (!NodeTemplate) continue;
			const FTransform NewRelativeTransform = NodeTemplate->GetRelativeTransform().GetRelativeTransform(Pivot);
			NodeTemplate->SetRelativeTransform_Direct(NewRelativeTransform);
		}
	}
};
void CreateBlueprintFromActors_Internal(UBlueprint* Blueprint, const TArray<AActor*>& Actors, const TFunctionRef<void(const FBlueprintAssemblyPropsTool&)>& AssembleBlueprintFunc)
{
	check(Actors.Num());
	check(Blueprint);
	check(Blueprint->SimpleConstructionScript);
	USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
	
	FBlueprintAssemblyPropsTool AssemblyProps(Blueprint, Actors);

	TMap<AActor*, TArray<AActor*>>& AttachmentMap = AssemblyProps.AttachmentMap;
	TArray<AActor*>& RootActors = AssemblyProps.RootActors;
	
	FKismetEditorUtilities::IdentifyRootActors(Actors, RootActors, &AttachmentMap);

	FTransform NewActorTransform = FTransform::Identity;
	bool bRepositionTopLevelNodes = false;
	
	if (SCS->GetSceneRootComponentTemplate() == nullptr)
	{
		// If there is not one unique root actor then create a new scene component to serve as the shared root node
		if (RootActors.Num() != 1)
		{
			AssemblyProps.RootNodeOverride = SCS->CreateNode(USceneComponent::StaticClass(), TEXT("SharedRoot"));

			if(const auto CustomSharedRoot = Cast<USceneComponent>(AssemblyProps.RootNodeOverride->ComponentTemplate))
			{
				CustomSharedRoot->SetMobility(EComponentMobility::Static);
			}
			SCS->AddNode(AssemblyProps.RootNodeOverride);
		}
	}
	else
	{
		bRepositionTopLevelNodes = true;
	}

	AssembleBlueprintFunc(AssemblyProps);

	if (RootActors.Num() == 1)
	{
		NewActorTransform = RootActors[0]->GetTransform();
		if (bRepositionTopLevelNodes)
		{
			RepositionNodes(SCS->GetRootNodes(), NewActorTransform);
		}
	}
	else if (RootActors.Num() > 1)
	{
		{
			// Find Transport point for all selected actors
			const FBox BoundingBox = UMBActorFunctions::GetAllActorsTransportPointWithExtents(RootActors);
			FVector AverageLocation = BoundingBox.GetCenter();
			AverageLocation.Z = BoundingBox.GetCenter().Z - BoundingBox.GetExtent().Z;
			NewActorTransform.SetTranslation(AverageLocation);
		}

		if (bRepositionTopLevelNodes)
		{
			RepositionNodes(SCS->GetRootNodes(), NewActorTransform);
		}
		else
		{
			for (const USCS_Node* TopLevelNode : SCS->GetRootNodes())
			{
				if (USceneComponent* TestRoot = Cast<USceneComponent>(TopLevelNode->ComponentTemplate))
				{
					RepositionNodes(TopLevelNode->GetChildNodes(), NewActorTransform);
				}
			}
		}
	}

	FKismetEditorUtilities::CompileBlueprint(Blueprint);
}
UBlueprint* UMBToolSubsystem::HarvestBlueprintFromActors(const FName BlueprintName, UPackage* Package,const TArray<AActor*>& Actors) const
{
	FKismetEditorUtilities::FHarvestBlueprintFromActorsParams Params;
	Params.ParentClass = AActor::StaticClass();

	
	auto AssemblyFunction = [](const FBlueprintAssemblyPropsTool& AssemblyProps)
	{
		// Harvest the components from each actor and clone them into the SCS
		TArray<UActorComponent*> AllSelectedComponents;
		for (const AActor* Actor : AssemblyProps.Actors)
		{
			for (UActorComponent* ComponentToConsider : Actor->GetComponents())
			{
				if (ComponentToConsider && !ComponentToConsider->IsVisualizationComponent())
				{
					AllSelectedComponents.Add(ComponentToConsider);
				}
			}
		}

		TArray<TPair<USceneComponent*, FTransform>> SceneComponentOldRelativeTransforms;
		if (AssemblyProps.RootActors.Num() > 1)
		{
			SceneComponentOldRelativeTransforms.Reserve(AssemblyProps.RootActors.Num());
			// Convert the components relative transform to world 
			for (const AActor* Actor : AssemblyProps.RootActors)
			{
				USceneComponent* SceneComponent = Actor->GetRootComponent();
				SceneComponentOldRelativeTransforms.Emplace(SceneComponent, SceneComponent->GetRelativeTransform());
				SceneComponent->SetRelativeTransform_Direct(SceneComponent->GetComponentTransform());
			}
		}

		FKismetEditorUtilities::FAddComponentsToBlueprintParams AddComponentsParams;
		AddComponentsParams.HarvestMode = (AssemblyProps.Actors.Num() > 1 ? FKismetEditorUtilities::EAddComponentToBPHarvestMode::Havest_AppendOwnerName : FKismetEditorUtilities::EAddComponentToBPHarvestMode::Harvest_UseComponentName);
		AddComponentsParams.OptionalNewRootNode = AssemblyProps.RootNodeOverride;
		AddComponentsParams.bKeepMobility = true; //V.1.1.0

		FKismetEditorUtilities::AddComponentsToBlueprint(AssemblyProps.Blueprint, AllSelectedComponents, AddComponentsParams);

		// Replace the modified components to their relative transform
		for (const TPair<USceneComponent*, FTransform >& Pair : SceneComponentOldRelativeTransforms)
		{
			Pair.Key->SetRelativeTransform_Direct(Pair.Value);
		}
	};

	UBlueprint* Blueprint = nullptr;

	if (Actors.Num() > 0)
	{
		if (Package != nullptr)
		{
			Blueprint = FKismetEditorUtilities::CreateBlueprint(Params.ParentClass, Package, BlueprintName, EBlueprintType::BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), FName("HarvestFromActors"));
		}

		if (Blueprint != nullptr)
		{
			CreateBlueprintFromActors_Internal(Blueprint, Actors, AssemblyFunction);
		}
	}
	return Blueprint;
}


void UMBToolSubsystem::CreateAssetFromCurrentCollection() const
{
	//Get Collection Actors
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return;}
	
	// Load necessary modules
	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	//Create Path
	const FString BlueprintName = FString::Printf(TEXT("/Game/Collections/BP_%s"),*ActiveCollectionWindow.ToString());
	FString Name, PackageName;
	AssetToolsModule.Get().CreateUniqueAssetName(BlueprintName, TEXT(""), PackageName, Name);
	const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);

	// Create object and package
	UPackage* package = CreatePackage(*PackageName);
	const TObjectPtr<UBlueprint> Blueprint = HarvestBlueprintFromActors(FName(*Name), package, CollectionActors);
	
	//Save Package
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;
	UPackage::Save(package, Blueprint,*FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()),SaveArgs);
	
	//Broadcast to everywhere
	AssetRegistry.AssetCreated(Blueprint);
	if(Blueprint->GetOutermost()->MarkPackageDirty())
	{
		
		//Sync Content Browser
		TArray<UObject*> Objects;
		Objects.Add(Blueprint);
		ContentBrowserModule.Get().SyncBrowserToAssets(Objects);
	}
}

void UMBToolSubsystem::BatchCurrentCollection(EMBMeshConversionType InConversionType) const
{
	//Get Editor World
	if(!GEditor){return;}
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	//Get Collection Actors
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return;}


	
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

	for(const auto CurrentActor : CollectionActors)
	{
		const auto CurrentSMActor = Cast<AStaticMeshActor>(CurrentActor);
		if(!CurrentSMActor || !IsValid(CurrentSMActor)){return;}

		CurrentSMComp = CurrentSMActor->GetStaticMeshComponent();

		AddStaticMeshToMeshData();
	}

	if(MeshData.IsEmpty()){return;}
	
	//Spawn And Get An Actor
	FTransform NewActorTransform;
	FBox BoundingBox = UMBActorFunctions::GetAllActorsTransportPointWithExtents(CollectionActors);
	FVector AverageLocation = BoundingBox.GetCenter();
	AverageLocation.Z = BoundingBox.GetCenter().Z - BoundingBox.GetExtent().Z;
	NewActorTransform.SetTranslation(AverageLocation);

	const FText TransactionDescription = FText::FromString(TEXT("Collection Actor Instance Creation."));
	GEngine->BeginTransaction(TEXT("CollectionCreation"), TransactionDescription, nullptr);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	const auto SpawnedActor = EditorWorld->SpawnActor<AActor>(AActor::StaticClass(),NewActorTransform,SpawnParams);
	if(!SpawnedActor){return;}

	USceneComponent* CreatedRootComponent = NewObject<USceneComponent>(SpawnedActor,FName(TEXT("RootComponent")));
	if(!CreatedRootComponent){SpawnedActor->Destroy(); return;}
	CreatedRootComponent->bAutoRegister = true;
	SpawnedActor->SetRootComponent(CreatedRootComponent);
	SpawnedActor->AddInstanceComponent(CreatedRootComponent);
	SpawnedActor->AddOwnedComponent(CreatedRootComponent);
	SpawnedActor->SetActorTransform(NewActorTransform);
	CreatedRootComponent->SetFlags(RF_Transactional);
	
	const FString NewActorLabel = FString::Printf(TEXT("%s_Instance"),*ActiveCollectionWindow.ToString());
	SpawnedActor->SetActorLabel(NewActorLabel);
	SpawnedActor->GetRootComponent()->SetMobility(EComponentMobility::Static);

	//Create Components
	if(InConversionType == EMBMeshConversionType::Sm)
	{
		for(const auto CurrentMeshData : MeshData)
		{
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
			TObjectPtr<UClass> ISmClass = InConversionType == EMBMeshConversionType::Ism ? UInstancedStaticMeshComponent::StaticClass() : UHierarchicalInstancedStaticMeshComponent::StaticClass();

			const FName CompName = MakeUniqueObjectName(EditorWorld, UInstancedStaticMeshComponent::StaticClass(), FName(*CurrentMeshData.StaticMesh->GetName()));
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
	GEngine->EndTransaction();
	
	//Selected Newly Created Collection Actor
	if(UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(SpawnedActor,true);
	}
}

void UMBToolSubsystem::RunMergeToolForCurrentCollection() const
{
	if(SelectAllInCollection())
	{
		FGlobalTabmanager::Get()->TryInvokeTab(FName("MergeActors"));
	}
}

bool UMBToolSubsystem::SelectAllInCollection() const
{
	TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(GetActiveCollectionFolderPath());
	if(CollectionActors.IsEmpty()){return false;}
	UMBActorFunctions::FilterModularActors(CollectionActors);
	if(CollectionActors.IsEmpty()){return false;}
	
	if(UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetSelectedLevelActors(CollectionActors);
	}
	return true;
}

bool UMBToolSubsystem::DeleteCurrentCollection()
{
	//Get Collection Actors & If Exist, Delete Actors & Folder
	const TArray<AActor*> CollectionActors = UMBActorFunctions::GetAllActorsUnderTheFolderPath(GetActiveCollectionFolderPath());

	const FText TransactionDescription = FText::FromString(TEXT("Collection Deletion"));
	GEngine->BeginTransaction(TEXT("MaterialAssignment"), TransactionDescription, nullptr);

	if(!CollectionActors.IsEmpty())
	{
		const auto Folder = CollectionActors[0]->GetFolder();

		for(AActor* Actor : CollectionActors)
		{
			Actor->Modify();
			
			Actor->Destroy();
		}
		UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
		
		ToolData->Modify();
		FActorFolders::Get().DeleteFolder(*EditorWorld,Folder);
	}
	
	//Delete collection name
	ToolData->Modify();
	ToolData->Collections.RemoveAt(LastActiveCollectionIndex);
	ActiveCollectionWindow = FName();
	LastActiveCollectionIndex = -1;
	SaveMBToolDataToDisk();
	UEditorAssetLibrary::SaveLoadedAsset(ToolData);

	GEngine->EndTransaction();
	
	if(GetToolMainScreen().Get())
	{
		Cast<IMBMainScreenInterface>(GetToolMainScreen().Get())->CollectionRemoved();
	}
	
	return true;
}

const FModularBuildingAssetData* UMBToolSubsystem::GetModAssetRowWithAssetName(const FString& InName) const
{
	TArray<FName> RowNames = GetModularAssetData()->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		if (const FModularBuildingAssetData* FoundRow = GetModularAssetData()->FindRow<FModularBuildingAssetData>(RowName, TEXT(""), true))
		{
			const TSoftObjectPtr<UObject>& AssetRef = FoundRow->AssetReference;

			if (AssetRef.IsValid() && AssetRef->GetName().Equals(InName))
			{
				return FoundRow;
			}
		}
	}
	return nullptr;
}

void UMBToolSubsystem::SyncAssetInBrowser(const FString& InAssetName) const
{
	if (const FModularBuildingAssetData* ModAssetData = GetModAssetRowWithAssetName(InAssetName))
	{
		const TSoftObjectPtr<UObject>& AssetRef = ModAssetData->AssetReference;

		if (!AssetRef.IsNull())
		{
			if (UObject* LoadedAsset = AssetRef.LoadSynchronous())
			{
				TArray<UObject*> ObjectsToSync = { LoadedAsset };

				FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
				ContentBrowserModule.Get().SyncBrowserToAssets(ObjectsToSync);
			}
		}
	}

}

void UMBToolSubsystem::OpenAsset(const FString& InAssetName) const
{
	if (const FModularBuildingAssetData* ModAssetData = GetModAssetRowWithAssetName(InAssetName))
	{
		const TSoftObjectPtr<UObject>& AssetRef = ModAssetData->AssetReference;

		if (!AssetRef.IsNull())
		{
			if (UObject* LoadedAsset = AssetRef.LoadSynchronous())
			{
				if (UAssetEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
				{
					EditorSubsystem->OpenEditorForAsset(LoadedAsset);
				}
			}
		}
	}
}


void UMBToolSubsystem::HandleOnAssetRemoved(const FAssetData& InAsset) const
{
	if(!ModularAssetData){return;}

	TArray<FName> RowNames = ModularAssetData->GetRowNames();
	FName RowToRemove;
	
	for (auto RowName : RowNames)
	{
		if(InAsset.AssetName.IsEqual(RowName))
		{
			RowToRemove = RowName;
			break;
		}
	}
	if (!RowToRemove.IsNone())
	{
		ModularAssetData->RemoveRow(RowToRemove);
		
		SaveModularAssetDataToDisk();
		
		if(MBToolMainScreen.IsValid())
		{
			Cast<IMBMainScreenInterface>(MBToolMainScreen.Get())->UpdateCategoryWindow();
		}

		const FString Dialog = FString::Printf(TEXT("Asset removed succesfully."));
		MBDebug::ShowNotifyInfo(Dialog);
	}
}

void UMBToolSubsystem::HandleOnAssetRenamed(const FAssetData& InAsset, const FString& InOldObjectPath) const
{
	if(!ModularAssetData){return;}
	const TArray<FName> RowNames = ModularAssetData->GetRowNames();
	if(RowNames.Num() == 0){return;}
	
	//Get Asset's Old Name
	FString OldAssetRowName = InOldObjectPath;
	int32 LastDotIndex;
	OldAssetRowName.FindLastChar('.', LastDotIndex); 
	if (LastDotIndex != INDEX_NONE)
	{
		OldAssetRowName.RightChopInline(LastDotIndex + 1, EAllowShrinking::No); 
	}
	
	//If Asset Exist On Tool
	if(RowNames.Contains(FName(*OldAssetRowName)))
	{
		if(const auto FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(FName(*OldAssetRowName), "", true))
		{
			FModularBuildingAssetData NewModularBuildingAssetData = *FoundRow;
			NewModularBuildingAssetData.AssetReference = Cast<UStaticMesh>(InAsset.GetAsset());
			ModularAssetData->RemoveRow(FName(*OldAssetRowName));
			ModularAssetData->AddRow(InAsset.AssetName,NewModularBuildingAssetData);
		}
	}
}


void UMBToolSubsystem::AddNewAssetsToTheTool(FName InAssetType,TArrayView<FAssetData> InDroppedAssets) const
{
	int32 AssetNum = InDroppedAssets.Num();
	if(!ToolData || AssetNum <= 0){return;}

	TArray<FName> RowsToRemove;
	for(auto Asset : InDroppedAssets)
	{
		if(IsValid(Asset.GetAsset()))
		{
			TArray<FName> RowNames = ModularAssetData->GetRowNames();
		
			for (auto RowName : RowNames)
			{
				if(const auto FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(FName(RowName), "", true))
				{
					if (FoundRow->AssetReference.IsValid())
					{
						if(!IsValid(Asset.GetAsset()) || Asset.GetAsset()->GetName().Equals(FoundRow->AssetReference->GetName()))
						{
							RowsToRemove.Add(RowName);
						}
					}
				}
			}
		}
	}
	if(RowsToRemove.Num() > 0)
	{
		for(const auto RowToRemove : RowsToRemove)
		{
			ModularAssetData->RemoveRow(RowToRemove);
		}
	}
	for(auto Asset : InDroppedAssets)
	{
		if(IsValid(Asset.GetAsset()))
		{
			FModularBuildingAssetData NewToolAssetData;
			NewToolAssetData.MeshType = InAssetType;
			NewToolAssetData.MeshCategory = ToolData->LastActiveBuildingCategory;
			NewToolAssetData.AssetReference = Cast<UStaticMesh>(Asset.GetAsset());

			FName RowName = DataTableUtils::MakeValidName(*Asset.AssetName.ToString());
			ModularAssetData->AddRow(RowName,NewToolAssetData);
		}
	}
	
	SaveModularAssetDataToDisk();
	
	OnNewAssetsAdded.Broadcast();
	FString Dialog;
	if(AssetNum == 1)
	{
		Dialog = FString::Printf(TEXT("Succesfully added an asset to the tool"));
	}
	else
	{
		Dialog = FString::Printf(TEXT("Succesfully added %d assets to the tool"),AssetNum);
	}
	
	MBDebug::ShowNotifyInfo(FString(Dialog));
}

FModularBuildingAssetData* UMBToolSubsystem::GetModularAssetDataByName(const FString& InAssetName) const
{
	if (!ModularAssetData) return nullptr;

	for (const FName& RowName : ModularAssetData->GetRowNames())
	{
		if (FModularBuildingAssetData* FoundRow = ModularAssetData->FindRow<FModularBuildingAssetData>(RowName, TEXT(""), true))
		{
			const TSoftObjectPtr<UObject>& AssetRef = FoundRow->AssetReference;

			if (AssetRef.IsValid() && AssetRef->GetName().Equals(InAssetName))
			{
				return FoundRow;
			}
		}
	}

	return nullptr;
}


FName UMBToolSubsystem::GetActiveCollectionFolderPath() const
{
	if(LastActiveCollectionIndex < 0){return SandboxFolderPath;}
	return *FString::Printf(TEXT("Prefabs/%s"),
	*ToolData->Collections[LastActiveCollectionIndex].ToString());
}

void UMBToolSubsystem::OpenModularBuildingSettingsDataAsset() const
{
	
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(GetToolUserSettings());
}

void UMBToolSubsystem::DeleteAllModularActorOnTheLevel()
{
	auto ModularActors = UMBActorFunctions::GetAllModularActorsInWorld();
	TArray<FFolder> Folders;
	for(const auto ActorToDelete : ModularActors)
	{
		Folders.AddUnique(ActorToDelete->GetFolder());
		ActorToDelete->Destroy();
	}
	if(Folders.Num() > 0)
	{
		UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();

		for(auto CurrentFolder : Folders)
		{
			FActorFolders::Get().DeleteFolder(*EditorWorld,CurrentFolder);
		}
	}
}

void UMBToolSubsystem::ChangePlacementType() const
{
	if(!ToolData){return;}
	
	ToolData->BuildingSettingsData.PlacementType = NextPlacementType(ToolData->BuildingSettingsData.PlacementType);
	
	OnPlacementTypeChanged.Broadcast();
}

void UMBToolSubsystem::ResetTool()
{
	if(!ActiveCollectionWindow.IsNone())
	{
		if(MBToolMainScreen.IsValid())
		{
			if(const auto IMainScreen = Cast<IMBMainScreenInterface>(MBToolMainScreen.Get()))
			{
				IMainScreen->CollectionRemoved();
				ActiveCollectionWindow = FName();
			}
		}
	}
	LastActiveCollectionIndex = -1;
	
	ModularAssetData->EmptyTable();
	SaveModularAssetDataToDisk();
	
	ToolData->ResetToDefault();
	SaveMBToolDataToDisk();

	if(IsValid(ToolUserSettings))
	{
		ToolUserSettings->ResetSettingParams();
	}
	
	if(MBToolMainScreen.IsValid())
	{
		if(const auto IMainScreen = Cast<IMBMainScreenInterface>(MBToolMainScreen.Get()))
		{
			IMainScreen->ResetToolWindowInterface();
		}
	}
}



void UMBToolSubsystem::RemoveNullAssetRow(const FName& InRowName) const
{
	ModularAssetData->RemoveRow(InRowName);
	SaveModularAssetDataToDisk();
}








