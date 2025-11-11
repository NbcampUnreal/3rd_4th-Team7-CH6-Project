// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ObjectDistribution/ODDistributionBase.h"
#include "ActorGroupingUtils.h"
#include "Editor.h"
#include "EditorActorFolders.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "ODPresetData.h"
#include "ODToolSettings.h"
#include "Development/ODDebug.h"
#include "UI/ODToolWindow.h"
#include "ODToolSubsystem.h"
#include "PBDRigidsSolver.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/StaticMeshComponent.h"
#include "Modules/ModuleManager.h"
#include "Development/ODSpawnCenter.h"
#include "Kismet2/KismetDebugUtilities.h"
#include "Physics/Experimental/PhysScene_Chaos.h"

#define LOCTEXT_NAMESPACE "ObjectDistributionBase"

static const FName DISTRIBUTED_OBJECT_TAG = FName(TEXT("DistributedObject"));

void UODDistributionBase::Setup(UODToolWindow* InToolWindow)
{
	ToolWindow = InToolWindow;
	ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem)
	{
		UE_LOG(LogTemp,Error,TEXT("ToolSubsystem Not Found!"));
		return;
	}
	if(!ToolSubsystem->GetODToolSettings())
	{
		UE_LOG(LogTemp,Error,TEXT("Tool Settings Not Found!"));
		return;
	}
	
	//ObjectDistributionData = ToolSubsystem->ObjectDistributionData;
	ToolSubsystem->OnPresetLoaded.AddUObject(this,&UODDistributionBase::OnPresetLoaded);

	ToolSubsystem->GetODToolSettings()->bDrawSpawnBounds;

	bIsInMixerMode = ToolSubsystem->bInMixerMode;

	if(bIsInMixerMode)
	{
		if(ToolSubsystem->GetPresetMixerMapData().Num() < 2)
		{
			ToolSubsystem->ToggleMixerMode();
		}
	}
	
	CalculateTotalSpawnCount();
	
	LoadDistData();
}

//Load Inherited Distribution Data
void UODDistributionBase::LoadDistData()
{
}

void UODDistributionBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	//Dont delete in PIE
	if(IsInPie() || !IsValid(ToolSubsystem)){return;}

	if(GetDistributionType() != DistributionType)
	{
		ToolSubsystem->GetODToolSettings()->LastSelectedDistributionType = DistributionType;
		OnDistributionTypeChangedSignature.ExecuteIfBound(DistributionType);
	}
}

void UODDistributionBase::OnFinishDistributionPressed()
{
	FinishKillZCheck();
	
	if(!ToolSubsystem->CreatedDistObjects.IsEmpty())
	{
		if(ToolSubsystem->GetODToolSettings()->FinishingType.Equals("Keep"))
		{
			FinishDistAsKeepActors();
		}
		else if(ToolSubsystem->GetODToolSettings()->FinishingType.Equals("Batch"))
		{
			EODMeshConversionType TargetType = EODMeshConversionType::HIsm;
			if(ToolSubsystem->GetODToolSettings()->TargetType.Equals("SM Component"))
			{
				TargetType = EODMeshConversionType::Sm;	
			}
			else if(ToolSubsystem->GetODToolSettings()->TargetType.Equals("ISM Component"))
			{
				TargetType = EODMeshConversionType::Ism;	
			}
			ToolSubsystem->CreateInstanceFromDistribution(TargetType);
		}
		else
		{
			FinishDistAsRunningMergeToll();
		}
	}
	
	ToolSubsystem->CreatedDistObjects.Empty();
	ToolSubsystem->ObjectDataMap.Empty();
	ToolSubsystem->InitialRelativeLocations.Empty();
	ToolSubsystem->InitialRelativeRotations.Empty();

	ToolSubsystem->bIsSimulating = false;
	ToolSubsystem->bIsSimulationInProgress = false;
	
	OnAfterODRegenerated.ExecuteIfBound();
	OnFinishConditionChangeSignature.ExecuteIfBound(false);

}

void UODDistributionBase::FinishDistAsKeepActors() const
{
	if(ToolSubsystem->GetODToolSettings()->bDisableSimAfterFinish)
	{
		for(const auto CurrentObject : ToolSubsystem->CreatedDistObjects)
		{
			CurrentObject->GetStaticMeshComponent()->SetSimulatePhysics(false);
			CurrentObject->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
		}
	}
		
	const FName GeneratedPresetPath = GenerateUniquePresetPath(ToolSubsystem->CreatedDistObjects[0]);
		
	TArray<AActor*> Actors;
	for(const auto CurrentActor : ToolSubsystem->CreatedDistObjects)
	{
		if(IsValid(CurrentActor))
		{
			CurrentActor->SetFolderPath(GeneratedPresetPath);
			Actors.Add(CurrentActor);
		}
	}

	if(const auto GroupingUtilities = UActorGroupingUtils::Get())
	{
		const bool bIsGroupingActive = GroupingUtilities->IsGroupingActive();
		GroupingUtilities->SetGroupingActive(true);
		UActorGroupingUtils::Get()->GroupActors(Actors);
		GroupingUtilities->SetGroupingActive(bIsGroupingActive);
	}
		
	if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(Actors[0],true);
	}
	
}

void UODDistributionBase::FinishDistAsRunningMergeToll() const
{
	if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		EditorActorSubsystem->SelectNothing();
		
		TArray<AActor*> Actors;
		for(const auto CurrentActor : ToolSubsystem->CreatedDistObjects)
		{
			if(IsValid(CurrentActor))
			{
				Actors.Add(CurrentActor);
			}
		}
		
		EditorActorSubsystem->SetSelectedLevelActors(Actors);
		
		FGlobalTabmanager::Get()->TryInvokeTab(FName("MergeActors"));
	}
}

FName UODDistributionBase::GenerateUniquePresetPath(const AActor* InSampleActor) const
{
	if(!IsValid(InSampleActor) || !ToolSubsystem || !InSampleActor->GetWorld()){return FName();}

	TArray<FString> CollectedFolderNames;
	FActorFolders::Get().ForEachFolderWithRootObject(*InSampleActor->GetWorld(), InSampleActor->GetFolder().GetRootObject(), [&CollectedFolderNames](const FFolder& Folder)
	{
		CollectedFolderNames.Add(Folder.ToString());
		return true;
	});

	const FString PresetName = ToolSubsystem->GetLastSelectedPreset().ToString();

	int32 PresetVariations = 0;
	for(auto CurrentFName : CollectedFolderNames)
	{
		if(CurrentFName.Contains(PresetName))
		{
			++PresetVariations;
		}
	}

	if(PresetVariations < 2)
	{
		PresetVariations = 1;
	}
	
	return FName(*FString::Printf(TEXT("DistributedActors/%s/%s_V%s"),*PresetName,*PresetName,*FString::FromInt(PresetVariations)));
}

void UODDistributionBase::OnShuffleDistributionPressed()
{
	//Dont shuffle in PIE
	if(IsInPie()){return;}

	if(ToolSubsystem->bIsSimulationInProgress)
	{
		if(IsValid(ToolWindow)){ToolWindow->SimulationStoppedWithForcibly();}
		OnStopBtnPressed();
	}

	auto ShuffleArrayLambda = [](TArray<TObjectPtr<AStaticMeshActor>>& Array) {
		const int32 ArraySize = Array.Num();
		for (int32 i = 0; i < ArraySize - 1; ++i) {
			const int32 RandomIndex = FMath::RandRange(i, ArraySize - 1);
			if (RandomIndex != i) {
				Array.Swap(i, RandomIndex);
			}
		}
	};
	
	if(ToolSubsystem->CreatedDistObjects.Num() == 0){return;}

	ShuffleArrayLambda(ToolSubsystem->CreatedDistObjects);

	ReDesignObjects();
}


void UODDistributionBase::OnDeleteDistributionPressed()
{
	//Dont delete in PIE
	if(IsInPie()){return ;}
	
	if(ToolSubsystem->CreatedDistObjects.Num() == 0){return;}

	if(ToolSubsystem->bIsSimulationInProgress)
	{
		if(IsValid(ToolWindow)){ToolWindow->SimulationStoppedWithForcibly();}
		StopSimulationManually();
	}
	
	DestroyObjects();



	OnFinishConditionChangeSignature.ExecuteIfBound(false);
}

void UODDistributionBase::OnResumeSimulationPressed()
{
	if(IsInPie())
	{
		if(GUnrealEd->IsPlayingSessionInEditor())
		{
			GUnrealEd->SetPIEWorldsPaused(false);	
		}
	}
}

void UODDistributionBase::OnStartBtnPressed()
{
	if(!ToolSubsystem->bIsSimulationInProgress)
	{
		ToolSubsystem->bIsSimulationInProgress = true;
		StartKillZCheck();
	}
	
	if(!ToolSubsystem->bIsSimulating)
	{
		GetWorld()->GetPhysicsScene()->GetSolver()->StartingSceneSimulation();
		GetWorld()->GetPhysicsScene()->StartFrame();
		ToolSubsystem->bIsSimulating = true;
	}
	else
	{
		ToolSubsystem->bIsSimulating = false;
	}
}



void UODDistributionBase::OnStopBtnPressed()
{
	ToolSubsystem->bIsSimulating = false;
	ToolSubsystem->bIsSimulationInProgress = false;
	
	OnCreateDistributionPressed();

	FinishKillZCheck();
}

void UODDistributionBase::StartKillZCheck()
{
	bTraceForKillZ = true;
}

void UODDistributionBase::FinishKillZCheck()
{
	//ReleaseSimulatedActorReferences();
	bTraceForKillZ = false;
	KillZCheckTimer = 0.0f;
}

void UODDistributionBase::CheckForActorsInKillZ()
{
	if(!IsValid(ToolSubsystem)){return;}
	
	const int32 Num = ToolSubsystem->CreatedDistObjects.Num();
	if(Num == 0)
	{
		FinishKillZCheck();
		return;
	}

	for(int32 Index = 0 ; Index < Num ; Index++)
	{
		if(IsValid(ToolSubsystem->CreatedDistObjects[Num - Index - 1]))
		{
			if(ToolSubsystem->CreatedDistObjects[Num - Index - 1]->GetActorLocation().Z < ToolSubsystem->GetODToolSettings()->KillZ)
			{
				const auto FoundActor = ToolSubsystem->CreatedDistObjects[Num - Index - 1];

				ToolSubsystem->CreatedDistObjects.RemoveAt(Num - Index - 1);
				
				FoundActor->Destroy();
			}
		}
	}
}

void UODDistributionBase::ResetSimulation()
{
	const auto SpawnCenterTransform = ToolWindow->GetSpawnCenterRef()->GetTransform();
	
	const auto Num = ToolSubsystem->CreatedDistObjects.Num();

	for(int32 Index = 0  ; Index < Num ; Index++)
	{
		ToolSubsystem->CreatedDistObjects[Num - Index - 1]->SetActorLocation(SpawnCenterTransform.TransformPosition(ToolSubsystem->InitialRelativeLocations[Num - Index - 1]));
		ToolSubsystem->CreatedDistObjects[Num - Index - 1]->SetActorRotation(SpawnCenterTransform.TransformRotation(ToolSubsystem->InitialRelativeRotations[Num - Index - 1].Quaternion()));
	}
}

void UODDistributionBase::OnPauseSimulationPressed()
{
	if(GUnrealEd->IsPlayingSessionInEditor())
	{
		GUnrealEd->SetPIEWorldsPaused(true);	
	}
}

void UODDistributionBase::OnCreateDistributionPressed()
{
	//Dont Create in PIE
	if(IsInPie() || !ToolSubsystem){return ;}
	
	if(ToolSubsystem->CreatedDistObjects.Num() > 0){DestroyObjects();}

	ToolSubsystem->ObjectDataMap.Empty();

	if(TotalSpawnCount <= 0)
	{
		OnFinishConditionChangeSignature.ExecuteIfBound(false);
		return;
	}
	
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if(!EditorActorSubsystem) {return;}
	EditorActorSubsystem->SelectNothing();
	
	CreatePaletteObjects();
	
	ReDesignObjects();
	
	if(ToolWindow && ToolWindow.Get()->GetSpawnCenterRef())
	{
		EditorActorSubsystem->SetActorSelectionState(ToolWindow.Get()->GetSpawnCenterRef(),true);
	}
	
	OnFinishConditionChangeSignature.ExecuteIfBound(true);
}

void UODDistributionBase::CreatePaletteObjects()
{
	if(!ToolSubsystem){return;}
	int32 TotalSpawn = TotalSpawnCount;
	
	int32 TotalIndex = 0;
	int32 DistIndex = 0;
	
	for(const auto& ObjectDistData : ToolSubsystem->ObjectDistributionData) //For each static meshes
	{
		if(!ObjectDistData.ActiveStatus || !UODToolSubsystem::IsSoftObjectValidToUse(ObjectDistData.StaticMesh)){continue;}
		
		FDistObjectData CommonData = ObjectDistData;

		const int32 CurrentDataSpawnCount = CommonData.DistributionProperties.SpawnCount;
		
		if(CurrentDataSpawnCount > 0)
		{
			ToolSubsystem->ObjectDataMap.Add(DistIndex,CommonData);

			const auto LoadedSM = CommonData.StaticMesh.LoadSynchronous();
			if(!IsValid(LoadedSM)){continue;}

			for(int32 Index = 0 ; Index < CurrentDataSpawnCount ; Index++) //For each Related Static Mesh 
			{
				const TObjectPtr<AStaticMeshActor> SpawnedSMActor = SpawnAndGetStaticMeshActor();
				if(!SpawnedSMActor){continue;}
				SpawnedSMActor->GetStaticMeshComponent()->SetStaticMesh(LoadedSM);

				//Add New Material
				
				if(CommonData.bSelectRandomMaterial && UODToolSubsystem::IsSoftObjectValidToUse(CommonData.SecondRandomMaterial))
				{
					if(CommonData.bSelectMaterialRandomly)
					{
						if(FMath::RandBool())
						{
							if(UMaterialInterface* LoadedMaterial = CommonData.SecondRandomMaterial.LoadSynchronous())
							{
								SpawnedSMActor->GetStaticMeshComponent()->SetMaterial(0,LoadedMaterial);
							}
						}
					}
					else
					{
						if(const auto LoadedMaterial = CommonData.SecondRandomMaterial.LoadSynchronous())
						{
							SpawnedSMActor->GetStaticMeshComponent()->SetMaterial(0,LoadedMaterial);
						}
					}
				}
				
				SpawnedSMActor->GetStaticMeshComponent()->SetLinearDamping(CommonData.DistributionProperties.LinearDamping);
				SpawnedSMActor->GetStaticMeshComponent()->SetAngularDamping(CommonData.DistributionProperties.AngularDamping);
				SpawnedSMActor->GetStaticMeshComponent()->SetMassOverrideInKg("",CommonData.DistributionProperties.Mass,true);
				SpawnedSMActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
				SpawnedSMActor->GetStaticMeshComponent()->SetSimulatePhysics(ToolSubsystem->GetODToolSettings()->bSimulatePhysics);
				SpawnedSMActor->Tags.Add(FName(*FString::FromInt(DistIndex)));
				SpawnedSMActor->Tags.Add(DISTRIBUTED_OBJECT_TAG);
				SpawnedSMActor->SetFolderPath(FName(TEXT("DistributedActors")));
				TotalIndex++;
				
				FName Name = MakeUniqueObjectName(GetWorld(), AStaticMeshActor::StaticClass(), FName(*CommonData.StaticMesh->GetName()));
				SpawnedSMActor->SetActorLabel(Name.ToString());

				ToolSubsystem->CreatedDistObjects.Add(SpawnedSMActor);
			}
			DistIndex++;
		}

		TotalSpawn -= CurrentDataSpawnCount;
		if(TotalSpawn < 0)
		{
			TotalSpawn = 0;
		}
	}
}

void UODDistributionBase::OnSelectObjectsPressed()
{
	if(ToolSubsystem->CreatedDistObjects.IsEmpty()){return;}

	UE_LOG(LogTemp,Warning,TEXT("Select Objects"));
	
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	
	TArray<AActor*> Actors;
	for (auto StaticMeshActor : ToolSubsystem->CreatedDistObjects)
	{
		AActor* Actor = Cast<AActor>(StaticMeshActor); 
		if (IsValid(Actor))
		{
			Actors.Add(Actor); 
		}
	}
	
	EditorActorSubsystem->SelectNothing();
	EditorActorSubsystem->SetSelectedLevelActors(Actors);
}

void UODDistributionBase::LevelActorDeleted(AActor* InActor)
{
	if(IsInPie())
	{
		AActor* Test = InActor;

		if(const AActor* EditorActor = EditorUtilities::GetEditorWorldCounterpartActor(Test))
		{
			ActorsInKillZ.AddUnique(*EditorActor->GetName());
		}
	}
	else
	{
		if(ToolSubsystem && !ToolSubsystem->CreatedDistObjects.IsEmpty())
		{
			int32 FoundIndex = -1;
			const int32 Num = ToolSubsystem->CreatedDistObjects.Num();
			for(int32 Index = 0; Index < Num ; Index++)
			{
				if(ToolSubsystem->CreatedDistObjects[Index] == InActor)
				{
					FoundIndex = Index;
					break;
				}
			}
			if(FoundIndex >= 0)
			{
				ToolSubsystem->CreatedDistObjects.RemoveAt(FoundIndex);
			}
		}
	}
}

TArray<AStaticMeshActor*> UODDistributionBase::SpawnAndGetEmptyStaticMeshActors(const int32& InSpawnCount)
{
	TArray<AStaticMeshActor*> LocalObjects;
	
	for(int Index = 0; Index < InSpawnCount ; Index++)
	{
		LocalObjects.Add(SpawnAndGetStaticMeshActor());
	}
	return LocalObjects;
}

AStaticMeshActor* UODDistributionBase::SpawnAndGetStaticMeshActor()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	return EditorWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
}

void UODDistributionBase::DestroyObjects() const
{
	const int32 Num = ToolSubsystem->CreatedDistObjects.Num();
	for(int32 Index = 0; Index < Num ; Index++)
	{
		const auto LocalObject = ToolSubsystem->CreatedDistObjects[Num - Index -1];
		ToolSubsystem->CreatedDistObjects.RemoveAt(Num - Index -1);
		if(LocalObject && !LocalObject->IsActorBeingDestroyed())
		{
			LocalObject->Destroy();
		}
	}
	ToolSubsystem->CreatedDistObjects.Empty();
}

bool UODDistributionBase::CheckForSpotSuitability(TObjectPtr<AStaticMeshActor> InSmActor) const
{
	const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return false;}
	FHitResult HitResult;
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(InSmActor);

	FVector Origin = InSmActor->GetStaticMeshComponent()->Bounds.Origin;
	const float Radius = InSmActor->GetStaticMeshComponent()->Bounds.SphereRadius;
	
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	UKismetSystemLibrary::SphereTraceSingle(EditorWorld,Origin,Origin,Radius,TraceType,false,ActorToIgnore,EDrawDebugTrace::None,HitResult,true);
	
	if(HitResult.bBlockingHit)
	{
		for(int32 Index = 0; Index < ToolSubsystem->GetODToolSettings()->MaxCollisionTest ; Index++)
		{
			Origin = InSmActor->GetStaticMeshComponent()->Bounds.Origin;
			
			InSmActor->AddActorWorldOffset(FVector(FMath::RandRange(-1.0f,1.0f),FMath::RandRange(-1.0f,1.0f),FMath::RandRange(-1.0f,1.0f)).GetSafeNormal() * Radius * FMath::Sin(Index + 1.0f));
			
			UKismetSystemLibrary::SphereTraceSingle(EditorWorld,Origin,Origin,Radius,TraceType,false,ActorToIgnore,EDrawDebugTrace::None,HitResult,true);

			if(!HitResult.bBlockingHit)
			{
				return true;
			}
		}
		return false;
	}
	return true;
}

void UODDistributionBase::CalculateTotalSpawnCount()
{
	int32 TotalCount = 0;
	//int32 UniqueSlots = 0;

	for(auto& LocalData : ToolSubsystem->ObjectDistributionData)
	{
		if(LocalData.ActiveStatus && UODToolSubsystem::IsSoftObjectValidToUse(LocalData.StaticMesh))
		{
			TotalCount += LocalData.DistributionProperties.SpawnCount;
			//++UniqueSlots;
		}
	}
	
	TotalSpawnCount = TotalCount;
	
	OnTotalSpawnCountChanged.ExecuteIfBound(TotalSpawnCount);
}

int32 UODDistributionBase::GetInitialTotalCount() const
{
	int32 ActualTotalCount = 0;
	if(!ToolSubsystem){return ActualTotalCount;}
	
	for(auto& LocalData : ToolSubsystem->ObjectDistributionData)
	{
		if(UODToolSubsystem::IsSoftObjectValidToUse(LocalData.StaticMesh) && LocalData.ActiveStatus)
		{
			ActualTotalCount += LocalData.DistributionProperties.SpawnCount;
		}
	}

	return ActualTotalCount;
}

void UODDistributionBase::ReDesignObjects()
{
	CollidingObjects = 0;

	ToolSubsystem->InitialRelativeLocations.Empty();
	ToolSubsystem->InitialRelativeRotations.Empty();
	
	if(!ToolWindow.Get() || !ToolWindow.Get()->GetSpawnCenterRef()){return;}
	
	const int32 ObjectNum = ToolSubsystem->CreatedDistObjects.Num();

	if(ObjectNum == 0){return;}
	
	//Get Center Location
	FVector SpawnCenterLocation = FVector::ZeroVector;
	if(ToolWindow && ToolWindow.Get()->GetSpawnCenterRef())
	{
		SpawnCenterLocation = ToolWindow.Get()->GetSpawnCenterRef()->GetActorLocation();
	}
	
	//Set Location & Rotations
	for(int32 Index = 0; Index < ObjectNum ; Index++)
	{
		FVector CalculatedVec = CalculateLocation(Index,ObjectNum);
		if(const auto CreatedObject = ToolSubsystem->CreatedDistObjects[Index])
		{
			if(!CreatedObject->Tags.IsValidIndex(0)){continue;}
			
			const auto FoundData =  FindDataFromMap(CreatedObject->Tags[0]);
			if(!FoundData){continue;}

			ToolSubsystem->InitialRelativeLocations.Add(CalculatedVec); //For Spawn Center Rotations

			//Add Initial Rotation Differences
			if(ToolWindow.Get() && ToolWindow.Get()->GetSpawnCenterRef())
			{
				CalculatedVec = ToolWindow->GetSpawnCenterRef()->GetActorRotation().RotateVector(CalculatedVec);
			}
			
			CalculatedVec += SpawnCenterLocation;
			CreatedObject->SetActorLocation(CalculatedVec);
			
			if(ToolSubsystem->GetODToolSettings()->bTestForCollider)
			{
				if(!CheckForSpotSuitability(CreatedObject))
				{
					++CollidingObjects;
				}
			}
			
			//Calculate initial rotation and set it 
			const auto CalculatedRot = CalculateRotation(CalculatedVec,SpawnCenterLocation,FoundData->DistributionProperties.OrientationType);
			CreatedObject->SetActorRotation(CalculatedRot);
			ToolSubsystem->InitialRelativeRotations.Add(ToolWindow->GetSpawnCenterRef()->GetTransform().InverseTransformRotation(CalculatedRot.Quaternion()).Rotator());
			
			CreatedObject->SetActorScale3D(FVector::OneVector * FMath::RandRange(FoundData->DistributionProperties.ScaleRange.X,FoundData->DistributionProperties.ScaleRange.Y));
			CreatedObject->GetStaticMeshComponent()->SetAbsolute(false,false,true);
		}
	}
	OnAfterODRegenerated.ExecuteIfBound();
}

void UODDistributionBase::AddSpawnCenterMotionDifferences(const FVector& InVelocity) const
{
	if(ToolSubsystem && !ToolSubsystem->CreatedDistObjects.IsEmpty())
	{
		for(const auto CurrentObject : ToolSubsystem->CreatedDistObjects)
		{
			if(CurrentObject){CurrentObject->AddActorWorldOffset(InVelocity);}
		}
	}
}


void UODDistributionBase::ReRotateObjectsOnSpawnCenter() const
{
	if(ToolSubsystem && !ToolSubsystem->CreatedDistObjects.IsEmpty())
	{
		if(ToolWindow && ToolWindow->GetSpawnCenterRef())
		{
			const int32 Num = ToolSubsystem->CreatedDistObjects.Num();
			for(int32 Index = 0; Index < Num ; Index++)
			{
				if(const auto CurrentObject = ToolSubsystem->CreatedDistObjects[Index])
				{
					if(ToolSubsystem->InitialRelativeLocations.IsValidIndex(Index))
					{
						FVector TargetLoc = ToolWindow->GetSpawnCenterRef()->GetActorRotation().RotateVector(ToolSubsystem->InitialRelativeLocations[Index]) + ToolWindow->GetSpawnCenterRef()->GetActorLocation();
						CurrentObject->SetActorLocation(TargetLoc);

						//Apply rotation differences
						CurrentObject->SetActorRotation(FRotator(FQuat(ToolSubsystem->InitialRelativeRotations[Index]) * FQuat(ToolWindow->GetSpawnCenterRef()->GetActorRotation())));
					}
				}
			}
		}
	}
}

FVector UODDistributionBase::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	return FVector::ZeroVector;
}

FRotator UODDistributionBase::CalculateRotation(const FVector& InLocation,const FVector& TargetLocation,const EObjectOrientation& Orientation)
{
	if(Orientation == EObjectOrientation::Inside)
	{
		return FRotationMatrix::MakeFromX(TargetLocation - InLocation).Rotator();

	}
	if(Orientation == EObjectOrientation::Outside)
	{
		return FRotationMatrix::MakeFromX(InLocation - TargetLocation).Rotator();

	}
	if(Orientation == EObjectOrientation::RandomZ)
	{
		FRotator RandRot;
		RandRot.Yaw = FMath::FRand() * 360.f;
		RandRot.Pitch = 0.0f;
		RandRot.Roll = 0.0f;
		return RandRot;
	}
	if(Orientation == EObjectOrientation::Random)
	{
		FRotator RandRot;
		RandRot.Yaw = FMath::FRand() * 360.f;
		RandRot.Pitch = FMath::FRand() * 360.f;
		RandRot.Roll = FMath::FRand() * 360.f;
		return RandRot;
	}
	return FRotator::ZeroRotator;
}


FDistObjectData* UODDistributionBase::FindDataFromMap(const FName& InIndexName) const
{
	if(FDistObjectData* FoundData = ToolSubsystem->ObjectDataMap.Find(FCString::Atoi(*InIndexName.ToString())))
	{
		return FoundData;
	}
	return nullptr;
}

void UODDistributionBase::SetSimulatePhysics() const
{
	auto Objects = ToolSubsystem->CreatedDistObjects;

	for(const auto Object : Objects)
	{
		Object->GetStaticMeshComponent()->SetSimulatePhysics(ToolSubsystem->GetODToolSettings()->bSimulatePhysics);
	}
}

bool UODDistributionBase::IsInPie()
{
	if (GEditor == nullptr){return true;}

	if(GEditor->GetPIEWorldContext())
	{
		return true;

	}
	return false;
}

void UODDistributionBase::CheckForKillZInSimulation()
{
	if(SimulatedActors.IsEmpty() && IsThereAnySimActor)
	{
		CollectAllSimulatedActors();
	}
	else
	{
		for(const auto FoundSMActor : SimulatedActors)
		{
			if(FoundSMActor->GetActorLocation().Z < ToolSubsystem->GetODToolSettings()->KillZ)
			{
				if(const AActor* EditorActor = EditorUtilities::GetEditorWorldCounterpartActor(FoundSMActor))
				{
					ActorsInKillZ.AddUnique(*EditorActor->GetName());
					FoundSMActor->Destroy();
				}
			}
		}
	}
}

void UODDistributionBase::CollectAllSimulatedActors()
{
	IsThereAnySimActor = false;
	
	if(!GEditor->PlayWorld){return;}
	
	for (TActorIterator<AActor> ActorItr(GEditor->PlayWorld); ActorItr; ++ActorItr)
	{
		const auto LocalActor = *ActorItr;
		if(IsValid(LocalActor) && LocalActor->GetClass()->GetName().Equals("StaticMeshActor") && LocalActor->ActorHasTag(DISTRIBUTED_OBJECT_TAG))
		{
			if(LocalActor->GetRootComponent()->IsSimulatingPhysics())
			{
				if(const auto FoundSMActor = Cast<AStaticMeshActor>(LocalActor))
				{
					if(FoundSMActor->GetStaticMeshComponent()->GetStaticMesh())
					{
						SimulatedActors.Add(FoundSMActor);
						IsThereAnySimActor = true;
					}
				}
			}
		}
	}
}

void UODDistributionBase::ReleaseSimulatedActorReferences()
{
	SimulatedActors.Empty();

	IsThereAnySimActor = true;
}


void UODDistributionBase::HandleBeginPIE()
{
	bTraceForKillZ = true;
}

void UODDistributionBase::HandleEndPIE()
{
	ReleaseSimulatedActorReferences();
	bTraceForKillZ = false;
	KillZCheckTimer = 0.0f;
	DestroyKillZActors();
}

void UODDistributionBase::DestroyKillZActors()
{
	if(ActorsInKillZ.Num() > 0)
	{
		if(ToolSubsystem)
		{
			ToolSubsystem->DestroyKillZActors(ActorsInKillZ);
		}
	}
	ActorsInKillZ.Empty();
}




void UODDistributionBase::OnPresetLoaded()
{
	CalculateTotalSpawnCount();
}


void UODDistributionBase::KeepSimulationChanges()
{
	auto CopyPropertiesFromSimToWorld = [] (AActor* ActorIt)
	{
		AActor* SimWorldActor = CastChecked<AActor>( ActorIt);
		if(!SimWorldActor){return;}
		
		// Find our counterpart actor
		AActor* EditorWorldActor = EditorUtilities::GetEditorWorldCounterpartActor( SimWorldActor );
		if(IsValid(EditorWorldActor) && IsValid(SimWorldActor))
		{
			constexpr auto CopyOptions = static_cast<EditorUtilities::ECopyOptions::Type>(EditorUtilities::ECopyOptions::CallPostEditChangeProperty |
			EditorUtilities::ECopyOptions::CallPostEditMove |
			EditorUtilities::ECopyOptions::OnlyCopyEditOrInterpProperties |
			EditorUtilities::ECopyOptions::FilterBlueprintReadOnly);
			EditorUtilities::CopyActorProperties( SimWorldActor, EditorWorldActor, CopyOptions );
		}
	};
	
	for (TActorIterator<AActor> It(GEditor->PlayWorld); It; ++It)
	{
		AActor* CurrentActor = *It;

		// Ensure the actor is valid
		if(IsValid(CurrentActor) && CurrentActor->ActorHasTag(DISTRIBUTED_OBJECT_TAG))
		{
			if(CurrentActor->GetRootComponent()->IsSimulatingPhysics())
			{
				CopyPropertiesFromSimToWorld(CurrentActor);
			}
		}
	}
}

void UODDistributionBase::OnSelectSpawnCenterPressed()
{
	if(!ToolWindow || !ToolWindow->GetSpawnCenterRef()){FReply::Handled();}

	if(ToolSubsystem->bIsSimulationInProgress)
	{
		if(IsValid(ToolWindow)){ToolWindow->SimulationStoppedWithForcibly();}
		OnStopBtnPressed();
	}
	
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	EditorActorSubsystem->SelectNothing();
	EditorActorSubsystem->SetActorSelectionState(ToolWindow->GetSpawnCenterRef(),true);
}

void UODDistributionBase::OnMoveSpawnCenterToWorldOriginPressed()
{
	if(ToolSubsystem->bIsSimulationInProgress)
	{
		if(IsValid(ToolWindow)){ToolWindow->SimulationStoppedWithForcibly();}
		OnStopBtnPressed();
	}
	
	if(ToolWindow && ToolWindow->GetSpawnCenterRef())
	{
		Cast<AODSpawnCenter>(ToolWindow->GetSpawnCenterRef())->SetActorLocation(FVector::ZeroVector);

		OnSelectSpawnCenterPressed();
	}
}

void UODDistributionBase::OnMoveSpawnCenterToCameraPressed()
{
	if(!ToolWindow){FReply::Handled();}
	
	ToolWindow->GetSpawnCenterToCameraView();
	
	OnSelectSpawnCenterPressed();
}

void UODDistributionBase::NatureTick(const float& InDeltaTime)
{
	if(bTraceForKillZ)
	{
		KillZCheckTimer += InDeltaTime;

		if(KillZCheckTimer > 0.5f)
		{
			CheckForActorsInKillZ();
			
			KillZCheckTimer = 0.0f;
		}
	}
	
	if(IsValid(ToolSubsystem) && ToolSubsystem->bIsSimulating)
	{
		GetWorld()->GetPhysicsScene()->GetSolver()->AdvanceAndDispatch_External(InDeltaTime);
	}
}

void UODDistributionBase::StopSimulationManually()
{
	ToolSubsystem->bIsSimulating = false;
	ToolSubsystem->bIsSimulationInProgress = false;
	FinishKillZCheck();
}


void UODDistributionBase::BeginDestroy()
{
	ToolWindow = nullptr;

	if(ToolSubsystem && ToolSubsystem->OnPresetLoaded.IsBound())
	{
		ToolSubsystem->OnPresetLoaded.RemoveAll(this);
	}

	FEditorDelegates::BeginPIE.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);
	
	UObject::BeginDestroy();
}

#undef LOCTEXT_NAMESPACE