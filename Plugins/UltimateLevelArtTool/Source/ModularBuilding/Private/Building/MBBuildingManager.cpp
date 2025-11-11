// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Building/MBBuildingManager.h"
#include "ActorGroupingUtils.h"
#include "Development/MBDebug.h"
#include "Data/MBModularAssetData.h"
#include "MBToolSubsystem.h"
#include "Editor.h" 
#include "LevelEditorActions.h"
#include "LevelEditorViewport.h" 
#include "MBActorFunctions.h"
#include "MBExtendedMath.h"
#include "MBToolData.h"
#include "MBUserSettings.h"
#include "SEditorViewport.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "MBToolAssetData.h"
#include "ModularBuilding.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Materials/MaterialInstanceDynamic.h" 
#include "Materials/MaterialInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/BodySetup.h"
#include "Settings/LevelEditorViewportSettings.h"

#pragma  region Initialization

void UMBBuildingManager::InitializeManager()
{
	if (!ToolSettings){ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();}

	if(ToolSettings)
	{
		if(UObject* ToolWindowObject = MBToolAssetData::PlacementMaterialPath.TryLoad())
		{
			if(const auto PlacementMaterialInterface = Cast<UMaterialInterface>(ToolWindowObject))
			{
				PlacementDynamicMaterial = UMaterialInstanceDynamic::Create(PlacementMaterialInterface, this);
			}
		}
		if(UObject* ToolWindowObject = MBToolAssetData::PlacementNaniteMaterialPath.TryLoad())
		{
			if(const auto PlacementNaniteMaterialInterface = Cast<UMaterialInterface>(ToolWindowObject))
			{
				PlacementDynamicMaterialForNanite = UMaterialInstanceDynamic::Create(PlacementNaniteMaterialInterface, this);
			}
		}
		ToolSettings->OnPlacementTypeChanged.AddDynamic(this,&UMBBuildingManager::OnPlacementTypeChanged);
	}
}



void UMBBuildingManager::ResetPropSnapRotation()
{
	PropTargetRotation = FRotator::ZeroRotator;
	if(MovedActor)
	{
		MovedActor->SetActorRotation(PropTargetRotation);
	}
}

void UMBBuildingManager::Tick( float DeltaTime )
{
	if ( LastFrameNumberWeTicked == GFrameCounter ){return;}


	if (!ToolSettings){ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();}
	if(!MovedActor || !ToolSettings){return;}
	
	DoTrace(GlobalHitResult);

	if(ToolSettings->PlacementMode == EPlacementMode::MultiModular){UpdateMultiPlacement();return;} //Multiplacement
	
	//Placement Tick
	if(GlobalHitResult.bBlockingHit)
	{
		HitActor = GlobalHitResult.GetActor();
	
		if(ToolSettings->PlacementMode == EPlacementMode::MultiModular){return;} //Multiplacement
	
		HitCategory = UMBActorFunctions::GetBuildingCategory(HitActor);
	
		if(HitCategory == EBuildingCategory::Modular && ToolSettings->MovedAssetCategory == EBuildingCategory::Modular && ToolSettings->GetToolData()->BuildingSettingsData.bEnableModularSnapping)
		{
			FindAndPlaceModular();
		}
		else
		{
			FindAndPlaceFree();
		}
		
		SetModActorRotation();
	}
	else // Added for V 1.1.0
	{
		PlaceInTheFieldOfView();
		
		SetModActorRotation();
	}
	
	LastFrameNumberWeTicked = GFrameCounter;
}

void UMBBuildingManager::DoTrace(FHitResult& OutResult) const
{
	if(!GCurrentLevelEditingViewportClient || !ToolSettings){return;}

	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams CollisionParams(lineTraceSingleName);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;
	CollisionParams.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	CollisionParams.AddIgnoredActor(MovedActor);
	const FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	
	const FVector StartLoc = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetOrigin();
	const FVector EndLoc = StartLoc + GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection() * ToolSettings->GetToolUserSettings()->ObjectTracingDistance;
	GWorld->LineTraceSingleByObjectType(OutResult, StartLoc, EndLoc, ObjectQueryParams, CollisionParams);
}


#pragma  endregion Initialization

#pragma  region AssetGetters

TArray<FModularBuildingAssetData*> UMBBuildingManager::GetModularAssetData() const
{
	TArray<FName> RowNames = ToolSettings->GetModularAssetData()->GetRowNames();
	TArray<FModularBuildingAssetData*> Assets;
	ToolSettings->GetModularAssetData()->GetAllRows("",Assets);
	return Assets;
}

#pragma  endregion AssetGetters

#pragma  region AssetCreation

void UMBBuildingManager::CreateAssetAndStartPlacementProgress(const FString& AssetName)
{
	DestroyMovedActor();
	
	if (!ToolSettings){ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();}

	if(ToolSettings->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
	{
		MovedActor  = CreateModActor(AssetName,GenerateModActorSpawnLocation(),TargetRotation,EBuildingCategory::Modular);
	}
	else
	{
		MovedActor  = CreateModActor(AssetName,GenerateModActorSpawnLocation(),PropTargetRotation,EBuildingCategory::Prop);
	}
	
	if(!MovedActor)
	{
		MBDebug::ShowMsgDialog(EAppMsgType::Ok,TEXT("Modular actor creation failed!"));
		return;
	}
	
	const auto ModAssetData = ToolSettings->GetModAssetRowWithAssetName(AssetName);
	ToolSettings->MovedAssetCategory = ModAssetData->MeshCategory;
	ChangePlacementMode(ModAssetData->MeshCategory);
	
	if(ModAssetData->MeshCategory == EBuildingCategory::Prop)
	{
		if(const auto MMComp = GetMMComponent(MovedActor))
		{
			if( ToolSettings->GetToolData()->PropBuildingSettingsData.bScaleRateCond)
			{
				MMComp->SetWorldScale3D(FVector::OneVector * ToolSettings->GetToolData()->PropBuildingSettingsData.ScaleRate);
			}
		}
	}
	
	SetModActorPlacementStatus(EPlacementStatus::None);
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	EditorActorSubsystem->SelectNothing();

	bPlacementInProgress = true;
}

TObjectPtr<AActor> UMBBuildingManager::CreateModActor(const FString& AssetName, const FVector& InLocation,const FRotator& InRotation,const EBuildingCategory InCategory) const
{
	if(AssetName.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("Asset Name is Empty"));return nullptr;}
	
	const auto ModAssetData = ToolSettings->GetModAssetRowWithAssetName(AssetName);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return nullptr;}
	const auto SpawnedActor = EditorWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),InLocation,InRotation,SpawnParams);

	SpawnedActor->Tags.Add(MODULAR_ACTOR_TAG);
	
	if(InCategory == EBuildingCategory::Modular)
	{
		SpawnedActor->Tags.Add(MODULAR_TAG);
	}
	else
	{
		SpawnedActor->Tags.Add(PROP_TAG);
	}
	
	if(!SpawnedActor){MBDebug::ShowMsgDialog(EAppMsgType::Ok,TEXT("Modular actor creation failed"));}
	SpawnedActor->GetStaticMeshComponent()->SetStaticMesh(ModAssetData->AssetReference.LoadSynchronous());
	SpawnedActor->SetActorLabel(ModAssetData->AssetReference->GetName());
	SpawnedActor->SetFolderPath(ToolSettings->GetActiveCollectionFolderPath());
	
	return SpawnedActor;
}

void UMBBuildingManager::GetRidOfFractals(FVector& InLocation)
{
	InLocation.X = FMath::RoundToFloat(InLocation.X);
	InLocation.Y = FMath::RoundToFloat(InLocation.Y);
	InLocation.Z = FMath::RoundToFloat(InLocation.Z);
}

#pragma  endregion AssetCreation

#pragma  region  UpdatingPlacement


void UMBBuildingManager::ApplyScaleRate()
{
	if(MovedActor && ToolSettings && ToolSettings->GetToolData())
	{
		if(const auto MMMeshComp = GetMMComponent(MovedActor))
		{
			if(ToolSettings->GetToolData()->PropBuildingSettingsData.bScaleRateCond)
			{
				MMMeshComp->SetWorldScale3D(FVector::OneVector * ToolSettings->GetToolData()->PropBuildingSettingsData.ScaleRate);
			}
			else
			{
				MMMeshComp->SetWorldScale3D(FVector::OneVector);
			}
			
		}
	}
}

void UMBBuildingManager::FindAndPlaceModular()
{
	if(!MovedActor || !HitActor || !GWorld){return;}
	
	//Find Cancelled Axis
	//const EMBAxis CancelledWorldAxis = FilterCancelledAxis();
	CancelledWorldAxis = FilterCancelledAxis();

	//Get Hit World Origin
	FVector HitWorldOrigin = GetMMWorldOrigin(HitActor);

	//Set Moved Actor World Location with same as Hit Location
	UMBExtendedMath::SetAxisOfVector(CancelledWorldAxis,HitWorldOrigin,UMBExtendedMath::GetAxisOfVector(CancelledWorldAxis,GlobalHitResult.Location)); //Changed by ref

	//Draw Origin To Hit
	if(const auto UserSettings = ToolSettings->GetToolUserSettings())
	{
		if(UserSettings->EnableDirectionDebugger)
		{
			UKismetSystemLibrary::DrawDebugLine(GWorld,HitWorldOrigin,GlobalHitResult.Location,UserSettings->DirectionDebugColor,0,UserSettings->DebugThickness);
		}
	}

	//For V.1.1.0 Changes
	//Set Location To Center in Hit Actor If Cursor is center of it 
	if(bReplacementModeActivated /*IsTheCursorCenterOfTheMesh(CancelledWorldAxis,HitWorldOrigin)*/) 
	{
		TargetLocation = HitActor->GetActorLocation();
	} 
	else // Else 
	{
		bool WorkingDir;
		EMBAxis WorkingAxis;
		
		//Calculate Nearest Axis
		FVector OriginToHit = (GlobalHitResult.Location - HitWorldOrigin);
		OriginToHit.Normalize();
		UMBExtendedMath::GetHighestAxisAndDirectionOfVector(OriginToHit,WorkingAxis,WorkingDir);
		
		CalculateAndSetModularTargetLocation(WorkingAxis,WorkingDir,CancelledWorldAxis);
		
		//Add Snap Offset
		if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->BuildingSettingsData.bSnapOffsetCond)
		{
			FVector OffsetVector = FVector::ZeroVector;
			
			UMBExtendedMath::SetAxisOfVector(WorkingAxis,OffsetVector,ToolSettings->GetToolData()->BuildingSettingsData.SnapOffset * (WorkingDir ? 1.0f : -1.0f));

			TargetLocation += OffsetVector;
		}
	
		//Add Z Offset
		if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->BuildingSettingsData.bZOffsetCond)
		{
			TargetLocation.Z += ToolSettings->GetToolData()->BuildingSettingsData.ZOffset;
		}
	}
	
	CheckForSpotAvailability();

	//Visibility Offset for V.1.1.0
	const float CancelledSign = FMath::Sign(UMBExtendedMath::GetAxisOfVector(CancelledWorldAxis,GlobalHitResult.Normal));
	FVector VisibilityOffset = FVector::ZeroVector;
	UMBExtendedMath::SetAxisOfVector(CancelledWorldAxis,VisibilityOffset,CancelledSign * 0.1f);
	
	GetRidOfFractals(TargetLocation);
	
	MovedActor->SetActorLocation(FMath::VInterpTo(MovedActor->GetActorLocation(),TargetLocation + VisibilityOffset,GWorld->DeltaTimeSeconds,13.0f));
}

EMBAxis UMBBuildingManager::FilterCancelledAxis() const
{
	const FVector NormalAbs =  GlobalHitResult.Normal.GetAbs();

	if(FMath::IsNearlyEqual(NormalAbs.X,1,0.1f))
	{	
		return EMBAxis::AxisX;
		
	}
	else if(FMath::IsNearlyEqual(NormalAbs.Y,1,0.1f))
	{
		return EMBAxis::AxisY;
	}
	return EMBAxis::AxisZ;
}

bool UMBBuildingManager::IsTheCursorCenterOfTheMesh(const EMBAxis& BlockedAxis,const FVector& InHitWorldOrigin) const
{
	FVector HitActorToOrigin = GetMMWorldOrigin(HitActor) - HitActor->GetActorLocation();
	
	UMBExtendedMath::SetAxisOfVector(BlockedAxis,HitActorToOrigin,0);
	HitActorToOrigin = HitActorToOrigin.GetAbs();
	float Alpha = (HitActorToOrigin.X + HitActorToOrigin.Y + HitActorToOrigin.Z) * 0.5f;
	Alpha = (GlobalHitResult.Location - InHitWorldOrigin).Size()/Alpha;
	return Alpha < 0.2;
}

void UMBBuildingManager::CalculateAndSetModularTargetLocation(const EMBAxis& InWorkingAxis, const bool& InWorkingDir,const EMBAxis& InBlockedAxis)
{
	const FVector HitActorFixedLoc = HitActor->GetActorLocation(); // Blocked axis 
	const EMBAxis SecondAxis = UMBExtendedMath::GetSecAxis(InWorkingAxis,InBlockedAxis);
	
	const float HitFirstAxisValue = GetMMMeshLengthInDirectionAndAxis(HitActor,InWorkingAxis,InWorkingDir);
	
	float MovedFirstAxisValue = 0.0f;
	
	if(!(InWorkingAxis == EMBAxis::AxisZ && InWorkingDir) && !IsMMMeshOnCenterOfAxis(MovedActor,InWorkingAxis))
	{
		if(!bRepositionMainAxis)
		{
			MovedFirstAxisValue = GetMMMeshLengthInDirectionAndAxis(MovedActor,InWorkingAxis,!InWorkingDir);
		}
	}

	FVector DirectionVec = UMBExtendedMath::GetAxisVectorWithEnumValue(InWorkingAxis);
	
	DirectionVec = DirectionVec * (InWorkingDir ? 1.0f : -1.0f); //Working Forward Vector
	
	TargetLocation = HitActorFixedLoc + (DirectionVec * (HitFirstAxisValue + MovedFirstAxisValue));
	
	//Second Axis Calculation
	FVector SecondAxisVector = FVector::ZeroVector;
	if(bSecondAxisPositionState)
	{
		if(GetMMMeshAxisStatus(MovedActor,SecondAxis) != GetMMMeshAxisStatus(HitActor,SecondAxis) &&
		FMath::IsNearlyEqual(UMBExtendedMath::GetAxisOfVector(SecondAxis,TargetLocation),UMBExtendedMath::GetAxisOfVector(SecondAxis,HitActor->GetActorLocation())))		{//FilterOffset
			SecondAxisVector = UMBExtendedMath::GetAxisVectorWithEnumValue(SecondAxis);
			SecondAxisVector *= GetMMMeshLengthInDirectionAndAxis(MovedActor,SecondAxis,!GetMMMeshAxisStatus(HitActor,SecondAxis));
			
			if(!GetMMMeshAxisStatus(HitActor,SecondAxis))
			{
				SecondAxisVector *= -1;
			}
		}
	}
	TargetLocation += SecondAxisVector;
}


void UMBBuildingManager::FindAndPlaceFree()
{
	float GridSize = 1.0f;
	if(IsGridEnabled())
	{
		GridSize = GEditor->GetGridSize();
	}

	TargetLocation = FVector(FMath::GridSnap(GlobalHitResult.Location.X,GridSize),FMath::GridSnap(GlobalHitResult.Location.Y,GridSize),FMath::GridSnap(GlobalHitResult.Location.Z,GridSize));

	//Add Z Offset


	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Modular)
	{
		if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->BuildingSettingsData.bZOffsetCond)
		{
			TargetLocation.Z += ToolSettings->GetToolData()->BuildingSettingsData.ZOffset;
		}
	}
	
	// if(ToolSettings->MovedAssetCategory == EBuildingCategory::Modular)
	// {
	// 	CheckForAdjacent();
	// }
	
	MovedActor->SetActorLocation(TargetLocation,false);
	
	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Prop)
	{
		if(ToolSettings->GetToolData()->PropBuildingSettingsData.bEnableSurfaceSnapping)
		{
			const auto ProjectedVector = FVector::VectorPlaneProject(MovedActor->GetActorForwardVector(), GlobalHitResult.Normal);
			PropTargetRotation = FRotationMatrix::MakeFromXZ(ProjectedVector, GlobalHitResult.Normal).Rotator();
			MovedActor->SetActorRotation(PropTargetRotation);
		}
	}
	
	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Prop)
	{
		if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->PropBuildingSettingsData.bZOffsetCond)
		{
			MovedActor->AddActorLocalOffset(FVector(0.0f,0.0f,ToolSettings->GetToolData()->PropBuildingSettingsData.ZOffset));
		}
	}
	
	CheckForSpotAvailability();
}

void UMBBuildingManager::PlaceInTheFieldOfView()
{
	if(!GCurrentLevelEditingViewportClient || !ToolSettings){return;}
	const FVector StartLoc = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetOrigin();
	const FVector EndLoc = StartLoc + GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection() * ToolSettings->GetToolUserSettings()->FreePlacementDistance;

	float GridSize = 1.0f;
	if(IsGridEnabled())
	{
		GridSize = GEditor->GetGridSize();
	}

	TargetLocation = FVector(FMath::GridSnap(EndLoc.X,GridSize),FMath::GridSnap(EndLoc.Y,GridSize),FMath::GridSnap(EndLoc.Z,GridSize));
	
	MovedActor->SetActorLocation(TargetLocation,false);

	SetModActorPlacementStatus(EPlacementStatus::Placeable);
}

void UMBBuildingManager::CheckForAdjacent()
{
	//Lock The Axis Of Adjacent
	if(LockedAxis == EMBAxis::None)
	{
		TraceOnSixDirection();
		return;
	}
	
	if(FMath::Abs(UMBExtendedMath::GetAxisOfVector(CancelledWorldAxis,GlobalHitResult.Location) - CancelledValue) <1000 && CancelledWorldAxis != EMBAxis::None)
	{
		UMBExtendedMath::SetAxisOfVector(LockedAxis,TargetLocation,LockedAxisValue);
	}
	else
	{
		LockedAxis = EMBAxis::None;
		LockedAxisValue = 0;
		CancelledWorldAxis = EMBAxis::None;
	}
}

void UMBBuildingManager::TraceOnSixDirection()
{
	if(!MovedActor){return;}

	TArray<TPair<EMBAxis,FVector>> LocMap;

	auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams CollisionParams(lineTraceSingleName);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;
	CollisionParams.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	CollisionParams.AddIgnoredActor(MovedActor);
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Visibility);

	FHitResult HitResult;

	TArray<FVector> FoundLocations;
	
	for(int32 Index = 0; Index < 6 ; ++Index)
	{
		FVector TraceDir = FVector::ZeroVector;
		EMBAxis Axis;
		
		if(Index == 0)
		{
			TraceDir =  FVector::XAxisVector;
			Axis = EMBAxis::AxisX;
		}
		else if(Index == 1)
		{
			TraceDir = FVector::XAxisVector * -1.0f;
			Axis = EMBAxis::AxisX;
		}
		else if(Index == 2)
		{
			TraceDir = FVector::YAxisVector;
			Axis = EMBAxis::AxisY;
		}
		else if(Index == 3)
		{
			TraceDir = FVector::YAxisVector * -1.0f;
			Axis = EMBAxis::AxisY;
		}
		else if(Index == 4)
		{
			TraceDir = FVector::ZAxisVector;
			Axis = EMBAxis::AxisZ;
		}
		else if(Index == 5)
		{
			TraceDir = FVector::ZAxisVector * -1.0f;
			Axis = EMBAxis::AxisZ;
		}

		FVector TraceStartLoc = GetMMWorldOrigin(MovedActor);
		FVector TraceEndLoc = TraceStartLoc + 5000.0f * TraceDir;
		
		EditorWorld->LineTraceSingleByObjectType(HitResult, TraceStartLoc, TraceEndLoc, ObjectQueryParams, CollisionParams);
		DrawDebugLine(EditorWorld,TraceStartLoc,TraceEndLoc,FColor::Blue);
		if(HitResult.bBlockingHit)
		{
			if(UMBActorFunctions::IsActorFromTool(HitResult.GetActor()))
			{
				if(UMBActorFunctions::GetBuildingCategory(HitResult.GetActor()) == EBuildingCategory::Modular)
				{
					LocMap.Add(TPair<EMBAxis,FVector>(Axis,HitResult.GetActor()->GetActorLocation()));
					CancelledValue = UMBExtendedMath::GetAxisOfVector(CancelledWorldAxis,HitResult.GetActor()->GetActorLocation());
				}
			}
		}
	}
	
	//Find Lowest Dist
	if(LocMap.IsEmpty()){return;}

	if(LocMap.Num() == 1)
	{
		LockedAxis  = LocMap[0].Key;
		LockedAxisValue = UMBExtendedMath::GetAxisOfVector(LockedAxis,LocMap[0].Value);
	}
		
	int32 LowestIndex = 0;
	float LowestDist = MAX_flt;

	for(int32 FoundIndex = 0 ; FoundIndex < FoundLocations.Num() ; ++FoundIndex)
	{
		float SqLength = FVector::DistSquared(LocMap[FoundIndex].Value,TargetLocation);
		if(SqLength < LowestDist)
		{
			LowestDist = SqLength;
			LowestIndex = FoundIndex;
		}
	}

	LockedAxis  = LocMap[LowestIndex].Key;
	LockedAxisValue = UMBExtendedMath::GetAxisOfVector(LockedAxis,LocMap[LowestIndex].Value);
}

void UMBBuildingManager::LockAxisOnFreePlacement()
{
	

	
	
}

#pragma endregion UpdatingPlacement

#pragma  region  Placement

bool UMBBuildingManager::LeftClickPressed()
{
	if(!MovedActor || !ToolSettings){return false;}

	if(ToolSettings->bIsCtrlPressed){return false;}

	if(!GCurrentLevelEditingViewportClient->Viewport->HasFocus())
	{
		return false;
	}

	//Place multiple actors and remove placeholders
	if(ToolSettings->PlacementMode == EPlacementMode::MultiModular)
	{
		PlaceMultiple();
		
		return false;
	}
	
	if(!ToolSettings->bIsMouseOnToolWindow)
	{
		if(ToolSettings->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
		{
			if(ToolSettings->GetToolData()->BuildingSettingsData.PlacementType == EPlacementType::Multiple)
			{
				//Start Multiple Placement
				MovedActor->SetActorRotation(TargetRotation);

				if((MultiPlacementBaseComponent = Cast<UInstancedStaticMeshComponent>(MovedActor->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(),true,FTransform(),false))))
				{
					if(const auto FoundSM = GetMMComponent(MovedActor)->GetStaticMesh())
					{
						MultiPlacementBaseComponent->SetStaticMesh(FoundSM);

						if(ToolSettings->GetToolUserSettings()->UsePreviewShader)
						{
							const auto MatSlotNames = MultiPlacementBaseComponent->GetMaterialSlotNames();
							const auto DynamicMaterial = FoundSM->NaniteSettings.bEnabled ? PlacementDynamicMaterialForNanite : PlacementDynamicMaterial;
							if(!MatSlotNames.IsEmpty())
							{
								for(const auto SlotName : MatSlotNames)
								{
									MultiPlacementBaseComponent->SetMaterialByName(SlotName,DynamicMaterial);
								}
							}
						}

						PlaceMultiple();
					}
				}
			}
			else
			{
				PlaceTheMovedActor(true);
			}
		}
		else
		{
			PlaceTheMovedActor(true);
		}
		return false;
	}
	return false;
}


void UMBBuildingManager::PlaceTheMovedActor(const bool& bRegenerate)
{
	const FText Transaction = NSLOCTEXT("ModularPlacementAction", "ModularPlacement", "Single Modular Actor Placement");
	const int32 TransactionIndex = GEditor->BeginTransaction(Transaction);
	
	if((!(PlacementStatus == EPlacementStatus::Placeable || PlacementStatus == EPlacementStatus::Replaceable)))
	{
		GEditor->CancelTransaction(TransactionIndex);
		return;
	}
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

	FRotator FinalRotation;
	if(ToolSettings->PlacementMode == EPlacementMode::SingleModular)
	{
		FinalRotation = TargetRotation;
	}
	else if (ToolSettings->PlacementMode == EPlacementMode::SingleProp)
	{
		FinalRotation = MovedActor->GetActorRotation();
	}
	
	const auto MovedAssetName = GetMMAssetName(MovedActor);
	
	TargetLocation = FVector(FMath::GridSnap(TargetLocation.X,1.0f),FMath::GridSnap(TargetLocation.Y,1.0f),FMath::GridSnap(TargetLocation.Z,1.0f));

	const auto CreatedModActor = CreateModActor(MovedAssetName,TargetLocation,FinalRotation,ToolSettings->MovedAssetCategory);

	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Prop)
	{
		if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->PropBuildingSettingsData.bZOffsetCond)
		{
			CreatedModActor->AddActorLocalOffset(FVector(0.0f,0.0f,ToolSettings->GetToolData()->PropBuildingSettingsData.ZOffset));
		}
	}

	if(!CreatedModActor)
	{
		UE_LOG(LogTemp,Error,TEXT("Actor Creation Failed!"));
		GEditor->CancelTransaction(TransactionIndex);
		return;
	}

	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Prop)
	{
		if(const auto MMComp = GetMMComponent(CreatedModActor))
		{
			if(ToolSettings->GetToolData()->PropBuildingSettingsData.bScaleRateCond)
			{
				MMComp->SetWorldScale3D(FVector::OneVector * ToolSettings->GetToolData()->PropBuildingSettingsData.ScaleRate);
			}
			else
			{
				MMComp->SetWorldScale3D(FVector::OneVector);
			}
		}
	}
	
	if(!bRegenerate)
	{
		DestroyMovedActor();
		bPlacementInProgress = false;
		ToolSettings->PlacementMode = EPlacementMode::None;
		ToolSettings->MovedAssetCategory = EBuildingCategory::None;
		ToolSettings->WorkingMode = EMBWorkingMode::None;
		bThirdAxisPositionState = false;
		bRepositionMainAxis = false;
		
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(CreatedModActor,true);
	}
	
	if(PlacementStatus == EPlacementStatus::Replaceable && OverlappedActor)
	{
		if(IsValid(OverlappedActor))
		{
			OverlappedActor->Destroy();
		}
		OverlappedActor = nullptr;
	}
	if(MovedActor){PlacementStatus = EPlacementStatus::None;}
	
	GEditor->EndTransaction();
}

void UMBBuildingManager::CancelPlacement()
{
	if(MovedActor && (EPlacementMode::SingleModular == ToolSettings->PlacementMode || EPlacementMode::SingleProp == ToolSettings->PlacementMode))
	{
		CancelSinglePlacement();
	}
	else if(EPlacementMode::MultiModular == ToolSettings->PlacementMode)
	{
		CancelMultiPlacement();
	}

	LockedAxis = EMBAxis::None;
}

void UMBBuildingManager::CancelSinglePlacement()
{
	if(UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		EditorActorSubsystem->SelectNothing();
	}
	DestroyMovedActor();
	bPlacementInProgress = false;
	ToolSettings->PlacementMode = EPlacementMode::None;
	ToolSettings->MovedAssetCategory = EBuildingCategory::None;
	ToolSettings->WorkingMode = EMBWorkingMode::None;
	bThirdAxisPositionState = false;
	bRepositionMainAxis = false;
}

void UMBBuildingManager::SetModActorPlacementStatus(const EPlacementStatus InNewPlacementStatus)
{
	if(InNewPlacementStatus == PlacementStatus){return;}
	PlacementStatus = InNewPlacementStatus;

	if(PlacementDynamicMaterial)
	{
		if(PlacementStatus == EPlacementStatus::Placeable)
		{
			PlacementDynamicMaterial->SetVectorParameterValue(FName(TEXT("Color")),ToolSettings->GetToolUserSettings()->PlaceableColor);
			PlacementDynamicMaterialForNanite->SetVectorParameterValue(FName(TEXT("Color")),ToolSettings->GetToolUserSettings()->PlaceableColor);
		}
		else if(PlacementStatus == EPlacementStatus::NotPlaceable)
		{
			PlacementDynamicMaterial->SetVectorParameterValue(FName(TEXT("Color")),ToolSettings->GetToolUserSettings()->NotPlaceableColor);
			PlacementDynamicMaterialForNanite->SetVectorParameterValue(FName(TEXT("Color")),ToolSettings->GetToolUserSettings()->NotPlaceableColor);

		}
		else if(PlacementStatus == EPlacementStatus::Replaceable)
		{
			PlacementDynamicMaterial->SetVectorParameterValue(FName(TEXT("Color")),ToolSettings->GetToolUserSettings()->ReplaceableColor);
			PlacementDynamicMaterialForNanite->SetVectorParameterValue(FName(TEXT("Color")),ToolSettings->GetToolUserSettings()->ReplaceableColor);
		}
	}


	if(ToolSettings->GetToolUserSettings()->UsePreviewShader)
	{
		if(const auto MovedStaticMesh = GetMMComponent(MovedActor))
		{
			const auto MatSlotNames = MovedStaticMesh->GetMaterialSlotNames();
	
			for(const auto SlotName : MatSlotNames)
			{
				if(MovedStaticMesh->GetStaticMesh())
				{
					MovedStaticMesh->SetMaterialByName(SlotName,MovedStaticMesh->GetStaticMesh()->NaniteSettings.bEnabled ? PlacementDynamicMaterialForNanite : PlacementDynamicMaterial);
				}
			}
		}
	}
}

void UMBBuildingManager::ChangePlacementMode(const EBuildingCategory& InNewBuildingCategory) const
{
	if(InNewBuildingCategory == EBuildingCategory::Modular)
	{
		ToolSettings->PlacementMode = EPlacementMode::SingleModular;
	}
	if(InNewBuildingCategory == EBuildingCategory::Prop)
	{
		ToolSettings->PlacementMode = EPlacementMode::SingleProp;
	}
}

void UMBBuildingManager::CheckForSpotAvailability()
{
	if(!MovedActor || !ToolSettings){return;}
	
	FVector Position = GetMMWorldOrigin(MovedActor);
	FHitResult HitResult;
	constexpr float Radius = 15; // constexpr
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(MovedActor);
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	UKismetSystemLibrary::SphereTraceSingle(GWorld,Position,Position,Radius,TraceType,false,ActorToIgnore,EDrawDebugTrace::None,HitResult,true);
	
	OverlappedActor = HitResult.GetActor();
	
	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Prop)
	{
		SetModActorPlacementStatus(EPlacementStatus::Placeable);
		return;
	}
	
	if(OverlappedActor && UMBActorFunctions::IsActorFromTool(OverlappedActor))
	{
		auto MovedType = GetModularType(MovedActor);
		auto OverlappedType = GetModularType(OverlappedActor);

		if(bReplacementModeActivated)
		{
			SetModActorPlacementStatus(EPlacementStatus::Replaceable);
		}
		else if(OverlappedActor->GetName().Equals(MovedActor->GetName()))
		{
			SetModActorPlacementStatus(EPlacementStatus::NotPlaceable);
		}
		else if(MovedType.IsEqual(OverlappedType))
		{
			//Same Asset
			SetModActorPlacementStatus(EPlacementStatus::Replaceable); //Changed from Replaceable to Not Replaceable
		}
		else
		{	// Not Same Type & Modular
			SetModActorPlacementStatus(EPlacementStatus::Replaceable);
		}
	}
	else // Hit Actor Not From Modular Asset
	{
		SetModActorPlacementStatus(EPlacementStatus::Placeable);
	}
}

void UMBBuildingManager::DestroyMovedActor()
{
	if(MovedActor)
	{
		if(!MovedActor->IsActorBeingDestroyed())
		{
			MovedActor->Destroy();
			MovedActor = nullptr;
		}
	}
}

void UMBBuildingManager::ResetRegularVariablesAfterPlacement()
{
	bThirdAxisPositionState = false;
}

#pragma  endregion  Placement

#pragma region  Rotation

void UMBBuildingManager::RotateActor(bool bInClockwise)
{
	if(!MovedActor){return;}

	if(ToolSettings->PlacementMode == EPlacementMode::SingleModular)
	{
		if(bInClockwise)
		{
			TargetRotation.Yaw = FMath::GridSnap(TargetRotation.Yaw + 90,90);
		}
		else
		{
			TargetRotation.Yaw = FMath::GridSnap(TargetRotation.Yaw - 90,90);
		}
	}
	else if (ToolSettings->PlacementMode == EPlacementMode::SingleProp)
	{
		float RotRate = 1.0f;
		if(ToolSettings->GetToolData()->PropBuildingSettingsData.bCustomRotationRateCond)
		{
			RotRate = ToolSettings->GetToolData()->PropBuildingSettingsData.CustomRotationRate;
		} 
		else if(FLevelEditorActionCallbacks::RotationGridSnap_IsChecked())
		{
			RotRate = GEditor->GetRotGridSize().Yaw;
		}
		MovedActor->AddActorLocalRotation(FRotator(0.0f,RotRate * (bInClockwise ? 1.0f : -1.0f ),0.0f));
	}
}



void UMBBuildingManager::SetModActorRotation() const
{
	if(!GWorld){return;}
	
	if(MovedActor && ToolSettings->MovedAssetCategory == EBuildingCategory::Modular)
	{
		const auto LocalTargetRotation = FMath::RInterpTo(MovedActor->GetActorRotation(), TargetRotation, GWorld->DeltaTimeSeconds, 12.0f);
		MovedActor->SetActorRotation(LocalTargetRotation);
	}
}

#pragma endregion  Rotation

#pragma region Helpers

FVector UMBBuildingManager::GenerateModActorSpawnLocation()
{
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return FVector::ZeroVector;}

	const FVector StartLoc = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetOrigin();
	const FVector EndLoc = StartLoc + GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection() * 1000000.0f;
	
	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams CollisionParams(lineTraceSingleName);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;
	CollisionParams.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Visibility);

	FHitResult HitResult;
	if(EditorWorld->LineTraceSingleByObjectType(HitResult, StartLoc, EndLoc, ObjectQueryParams, CollisionParams))
	{
		return HitResult.Location + GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection() * -200.0f;
	}
	return FVector::ZeroVector;
}



EBuildingCategory UMBBuildingManager::GetBuildingCategoryWithAssetName(const FString& InAssetName) const
{
	auto ModAssetData = GetModularAssetData();
	for(const auto CurrentModAsset : ModAssetData)
	{
		if(*CurrentModAsset->AssetReference->GetName() == *InAssetName)
		{
			return CurrentModAsset->MeshCategory;
		}
	}
	return EBuildingCategory::None;
}


FName UMBBuildingManager::GetModularType(const AActor* InModActor) const
{
	if(InModActor && UMBActorFunctions::IsActorFromTool(InModActor))
	{
		if(const auto Asset =  ToolSettings->GetModAssetRowWithAssetName(GetMMAssetName(InModActor)))
		{
			return Asset->MeshType;
		}
		UE_LOG(LogTemp,Error,TEXT("Not Found Modular Asset Data named with: %s. Please re-add this asset to the tool or remove the tag named ModularActor on it."),*GetMMAssetName(InModActor));
		return FName(TEXT(""));
	}
	return FName(TEXT(""));
}

void UMBBuildingManager::OnPlacementTypeChanged()
{
	if(ToolSettings->PlacementMode == EPlacementMode::MultiModular && ToolSettings->GetToolData()->BuildingSettingsData.PlacementType != EPlacementType::Multiple)
	{
		ToolSettings->PlacementMode = EPlacementMode::MultiModular;

		FString ObjectName;
		if(MovedActor)
		{
			ObjectName = GetMMAssetName(MovedActor);
		}
		
		CancelPlacement();
		CreateAssetAndStartPlacementProgress(ObjectName);
	}
}

bool UMBBuildingManager::IsGridEnabled() const
{
	if (const ULevelEditorViewportSettings* ViewportSettings = GetDefault<ULevelEditorViewportSettings>())
	{
		return ViewportSettings->GridEnabled;
	}
	return false;
}

#pragma endregion Helpers

#pragma region Inputs

bool UMBBuildingManager::EscPressed()
{
	if(bPlacementInProgress)
	{
		CancelPlacement();
		
		return true;
	}
	return false;
}

bool UMBBuildingManager::AltPressed(bool bIsPressed)
{
	if(bIsPressed)
	{
		if(bPlacementInProgress && ToolSettings && ToolSettings->PlacementMode  == EPlacementMode::SingleModular)
		{
			bReplacementModeActivated = true;
			return false;
		}
	}
	else
	{
		bReplacementModeActivated = false;
	}
	return false;
}

#pragma endregion Inputs

#pragma region Duplication

void UMBBuildingManager::SetNewDuplicationDataInBm(const FDuplicationData& InDuplicationData)
{
	SetReceivedDuplicationData(InDuplicationData);
	RegenerateTheDuplication();
	
}


void UMBBuildingManager::SetReceivedDuplicationData(const FDuplicationData& InDuplicationData)
{
	if(InDuplicationData.DupAxis == XDupData.DupAxis)
	{
		XDupData = InDuplicationData;
	}
	else if(InDuplicationData.DupAxis == YDupData.DupAxis)
	{
		YDupData = InDuplicationData;
	}
	else if(InDuplicationData.DupAxis == ZDupData.DupAxis)
	{
		ZDupData = InDuplicationData;
	}
}
void UMBBuildingManager::MakeDuplicationRectangle()
{
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

	const float Extent = FMath::Abs(GetMMLocalExtent(SelectedDupActor).X * 2); //Extent > 0

	const int32 XNum = XDupData.NumOfDup;
	const int32 YNum = YDupData.NumOfDup;
	
	TArray<AActor*> SpawnedActors;
	SpawnedActors.Add(SelectedDupActor);

	FRotator RefActorRot = SelectedDupActor->GetActorRotation();
	
	//First X Edge

	for(int32 i = 0; i < XNum - 1; i++)
	{
		FVector TargetLoc = SelectedDupActor->GetActorLocation() + SelectedDupActor->GetActorForwardVector() * ((i+1) * (Extent + XDupData.Offset));
		
		TObjectPtr<AActor> CreatedActor = GetDupActorFromPool(); 
		if(!CreatedActor)
		{
			CreatedActor = EditorActorSubsystem->DuplicateActor(SelectedDupActor,GEditor->GetEditorWorldContext().World());
		}
		CreatedActor->SetActorLocation(TargetLoc);
		CreatedActor->SetActorRotation(RefActorRot);
		CreatedActor->AttachToActor(SelectedDupActor,FAttachmentTransformRules::KeepWorldTransform);
		SpawnedActors.Add(CreatedActor);
	}

	//First Y Edge
	auto NextFollowedActor = SpawnedActors.Last();
	FVector EdgeLoc = NextFollowedActor->GetActorLocation() + NextFollowedActor->GetActorForwardVector() * Extent;
	RefActorRot =  NextFollowedActor->GetActorRotation() + FRotator (0.0f,-90.0f,0.0f);
	for(int32 i = 0; i < YNum; i++)
	{
		FVector TargetLoc = EdgeLoc +  RefActorRot.Vector() * i *  (Extent + YDupData.Offset);
		
		TObjectPtr<AActor> CreatedActor = GetDupActorFromPool(); 
		if(!CreatedActor)
		{
			CreatedActor = EditorActorSubsystem->DuplicateActor(SelectedDupActor,GEditor->GetEditorWorldContext().World());
		}
		
		CreatedActor->SetActorLocation(TargetLoc);
		CreatedActor->SetActorRotation(RefActorRot);
		CreatedActor->AttachToActor(SelectedDupActor,FAttachmentTransformRules::KeepWorldTransform);
		SpawnedActors.Add(CreatedActor);
	}

	//Second X Edge
	NextFollowedActor = SpawnedActors.Last();
	EdgeLoc = NextFollowedActor->GetActorLocation() + NextFollowedActor->GetActorForwardVector() *  Extent;
	RefActorRot =  NextFollowedActor->GetActorRotation() + FRotator (0.0f,-90.0f,0.0f);
	for(int32 i = 0; i < XNum; i++)
	{
		FVector TargetLoc = EdgeLoc +  RefActorRot.Vector() * i * (Extent + XDupData.Offset);
		
		TObjectPtr<AActor> CreatedActor = GetDupActorFromPool(); 
		if(!CreatedActor)
		{
			CreatedActor = EditorActorSubsystem->DuplicateActor(SelectedDupActor,GEditor->GetEditorWorldContext().World());
		}
		
		CreatedActor->SetActorLocation(TargetLoc);
		CreatedActor->SetActorRotation(RefActorRot);
		CreatedActor->AttachToActor(SelectedDupActor,FAttachmentTransformRules::KeepWorldTransform);
		SpawnedActors.Add(CreatedActor);
	}

	//Second Y Axis
	NextFollowedActor = SpawnedActors.Last();
	EdgeLoc = NextFollowedActor->GetActorLocation() + NextFollowedActor->GetActorForwardVector() *  Extent;
	RefActorRot =  NextFollowedActor->GetActorRotation() + FRotator (0.0f,-90.0f,0.0f);
	for(int32 i = 0; i < YNum; i++)
	{
		FVector TargetLoc = EdgeLoc +  RefActorRot.Vector() * i * (Extent + YDupData.Offset);
		
		TObjectPtr<AActor> CreatedActor = GetDupActorFromPool(); 
		if(!CreatedActor)
		{
			CreatedActor = EditorActorSubsystem->DuplicateActor(SelectedDupActor,GEditor->GetEditorWorldContext().World());
		}
		
		CreatedActor->SetActorLocation(TargetLoc);
		CreatedActor->SetActorRotation(RefActorRot);
		CreatedActor->AttachToActor(SelectedDupActor,FAttachmentTransformRules::KeepWorldTransform);
		SpawnedActors.Add(CreatedActor);
	}
	
	if(ZDupData.NumOfDup > 1)
	{
		TArray<AActor*> TempLocalHolder;
		for(const auto CurrentHolder : SpawnedActors)
		{
			const FVector ZDirectionVec = FVector::ZAxisVector * (ZDupData.bDirection ? 1.0f : -1.0f);
			TempLocalHolder.Append(SpawnDupPlaceHolders(ZDupData.NumOfDup - 1,ZDirectionVec,ZDupData.Offset,CurrentHolder,EditorActorSubsystem,EMBAxis::AxisZ,true));
		}
		SpawnedActors.Append(TempLocalHolder);
	}
	Placeholders.Append(SpawnedActors);
	SpawnedActors.Empty();
	bIsDuplicatingNow = false;
	
}

void UMBBuildingManager::RegenerateTheDuplication()
{
	if(!ToolSettings){return;}
	
	MoveAllObjectToThePool();

	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if(!EditorActorSubsystem){return;}
	
	if(!ToolSettings->bIsDuplicationInprogress && !SelectedDupActor)
	{
		const TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
		if(SelectedActors.Num() != 1)
		{
			return;
		}
		SelectedDupActor  = SelectedActors[0];
	}

	ToolSettings->bIsDuplicationInprogress = true;
	bIsDuplicatingNow = true;
	
	if(FilterDuplicationForRectangle())
	{
		MakeDuplicationRectangle();
		return;
	}

	Placeholders.Add(SelectedDupActor); //Add Selected Initially
	
	if(XDupData.NumOfDup > 0)
	{
		const FVector XDirectionVec = FVector::XAxisVector * (XDupData.bDirection ? 1.0f : -1.0f);
		Placeholders.Append(SpawnDupPlaceHolders(FMath::Clamp(XDupData.NumOfDup -1,0,XDupData.NumOfDup + 1),XDirectionVec,XDupData.Offset,SelectedDupActor,EditorActorSubsystem,EMBAxis::AxisX,true));
	}
	if(YDupData.NumOfDup > 0)
	{
		TArray<AActor*> TempLocalHolder;

		const uint16 YNum = Placeholders.Num();
		for(uint16 Index = 0; Index < YNum ; Index++)
		{
			const bool CanSpawn = FilterXDuplicationForHole(Index,YNum); //Filter For Wall
			
			const FVector YDirectionVec = FVector::YAxisVector * (YDupData.bDirection ? 1.0f : -1.0f);
			TempLocalHolder.Append(SpawnDupPlaceHolders(FMath::Clamp(YDupData.NumOfDup -1,0,YDupData.NumOfDup + 1),YDirectionVec,YDupData.Offset,Placeholders[Index],EditorActorSubsystem,EMBAxis::AxisY,CanSpawn));
		}
		Placeholders.Append(TempLocalHolder);
	}
	if(ZDupData.NumOfDup > 1)
	{
		TArray<AActor*> TempLocalHolder;
		for(const auto CurrentHolder : Placeholders)
		{
			const FVector ZDirectionVec = FVector::ZAxisVector * (ZDupData.bDirection ? 1.0f : -1.0f);
			TempLocalHolder.Append(SpawnDupPlaceHolders(ZDupData.NumOfDup - 1,ZDirectionVec,ZDupData.Offset,CurrentHolder,EditorActorSubsystem,EMBAxis::AxisZ,true));
		}
		Placeholders.Append(TempLocalHolder);
	}
	
	bIsDuplicatingNow = false;
}

TArray<AActor*> UMBBuildingManager::SpawnDupPlaceHolders(const int32& InNum, const FVector& InDir, const float& InOffset,AActor* ActorToFollow,UEditorActorSubsystem* InEditorActorSubsystem,EMBAxis DupAxis,bool bCanSpawn)
{
	if(!SelectedDupActor){return TArray<AActor*>();}
	
	TArray<AActor*> SpawnedActors;
	
	const int32 Count = InNum;
	for(int32 Index = 0; Index < Count ; Index++)
	{
		//Filters
		if(DupAxis == EMBAxis::AxisY && !bCanSpawn && !FilterYDuplicationForHole(Index,InNum))
		{
			continue;
		}
		
		FVector SelectedActorLocation = ActorToFollow->GetActorLocation();
		
		const float SelectedWorldExtent = GetMMHalfLengthOfAxis(ActorToFollow,UMBExtendedMath::GetAxisEnumOfDirectionVector(InDir)); //Pure World Extent
		
		FVector FirstOffset = FVector::ZeroVector; //
		
		FVector DirVec = (SelectedWorldExtent * InDir * 2) + (InDir * InOffset);
		EMBAxis OutAxis;
		bool OutDir;
		UMBExtendedMath::GetHighestAxisAndDirectionOfVector(InDir,OutAxis,OutDir);
		FirstOffset =( (InDir * SelectedWorldExtent * 2) + (InDir * InOffset));
		
		DirVec = DirVec * (Index + 0) + (FirstOffset + SelectedActorLocation);
		TObjectPtr<AActor> CreatedActor = GetDupActorFromPool(); //Get From Pool If Exist One
		if(!CreatedActor)
		{
			CreatedActor = InEditorActorSubsystem->DuplicateActor(ActorToFollow,GEditor->GetEditorWorldContext().World());
		}
		CreatedActor->SetActorLocation(DirVec);
		CreatedActor->SetActorRotation(ActorToFollow->GetActorRotation());
		CreatedActor->AttachToActor(SelectedDupActor,FAttachmentTransformRules::KeepWorldTransform);
		SpawnedActors.Add(CreatedActor);
	}
	return SpawnedActors;
}

void UMBBuildingManager::ApplyModularDuplicationFilterInBM(const FDuplicationFilters& InDuplicationFilters)
{
	DuplicationFilters = InDuplicationFilters;
	RegenerateTheDuplication();
}

TArray<FDuplicationData*> UMBBuildingManager::GetDuplicationData()
{
	TArray<FDuplicationData*> DupData =  TArray<FDuplicationData*> {&XDupData,&YDupData,&ZDupData};
	return DupData;
}

bool UMBBuildingManager::IsTheActorFromDuplicationPool(AActor* InActor) const
{
	if(!InActor){return false;}
	
	if(SelectedDupActor && SelectedDupActor == InActor)
	{
		return true;
	}
	if(!Placeholders.IsEmpty() && Placeholders.Contains(InActor))
	{
		return true;
	}
	if(!DuplicatedFreePool.IsEmpty() && DuplicatedFreePool.Contains(InActor))
	{
		return true;
	}
	return false;
	
}

void UMBBuildingManager::SelectTheDuplicationActor() const
{
	if(SelectedDupActor)
	{
		const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		EditorActorSubsystem->SetActorSelectionState(SelectedDupActor,true);
	}
}

void UMBBuildingManager::ApplyMaterialToDuplicatedActors(const int32& InSlotIndex, UMaterialInterface* InMaterialInterface)
{
	if(InSlotIndex < 0 || !InMaterialInterface){return;}

	const FText TransactionDescription = FText::FromString(TEXT("Duplication material change"));
	GEngine->BeginTransaction(TEXT("MaterialAssignment"), TransactionDescription, nullptr);
	
	if(Placeholders.Num() > 0)
	{
		for(const auto SelectedActor : Placeholders)
		{
			SelectedActor->Modify();
			
			if(const auto FoundMesh = GetMMComponent(SelectedActor))
			{
				FoundMesh->SetMaterial(InSlotIndex,InMaterialInterface);
			}
		}
	}
	if(DuplicatedFreePool.Num() > 0)
	{
		for(const auto SelectedActor : DuplicatedFreePool)
		{
			SelectedActor->Modify();
			
			if(const auto FoundMesh = GetMMComponent(SelectedActor))
			{
				FoundMesh->SetMaterial(InSlotIndex,InMaterialInterface);
			}
		}
	}
	GEngine->EndTransaction();
}

TObjectPtr<AActor> UMBBuildingManager::GetDupActorFromPool()
{
	if(!DuplicatedFreePool.IsEmpty())
	{
		const auto LocalPoolActor =  DuplicatedFreePool[DuplicatedFreePool.Num() -1];
		DuplicatedFreePool.RemoveAt(DuplicatedFreePool.Num() -1);
		return LocalPoolActor;
	}
	return nullptr;
}

void UMBBuildingManager::MoveAllObjectToThePool()
{
	if(Placeholders.IsEmpty()){return;}
	
	const auto Num = Placeholders.Num();
	for(int32 i = 0 ; i < Num ; i++)
	{
		const auto CurrentActor = Placeholders[i];
		if(CurrentActor != SelectedDupActor)
		{
			CurrentActor->SetActorLocation(FVector(0.0f,0.0f,+5000.0f));
			DuplicatedFreePool.AddUnique(CurrentActor);
		}
	}
	Placeholders.Empty();
}

void UMBBuildingManager::StopModularDuplication(bool bInApplyChanges)
{
	if(bInApplyChanges)
	{
		if(!Placeholders.IsEmpty() && SelectedDupActor)
		{
			const FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules::KeepWorldTransform;
			for(const auto CurrentActor : Placeholders)
			{
				UKismetSystemLibrary::TransactObject(CurrentActor);
				CurrentActor->DetachFromActor(DetachmentTransformRules);
				CurrentActor->SetFolderPath(ToolSettings->GetActiveCollectionFolderPath());
				UActorGroupingUtils::Get()->GroupActors(Placeholders);
			}
		}
		DeletePoolAndResetVariables();
	}
	else
	{
		for(const auto PoolActor : Placeholders)
		{
			if(PoolActor)
			{
				if(PoolActor != SelectedDupActor)
				{
					PoolActor->Destroy();
				}
			}
		}
		Placeholders.Empty();
		DeletePoolAndResetVariables();

	}
	ToolSettings->WorkingMode = EMBWorkingMode::None;
}

void UMBBuildingManager::DeletePoolAndResetVariables()
{
	if(!ToolSettings){return;}
	
	if(!DuplicatedFreePool.IsEmpty())
	{
		for(const auto LocalPoolActor : DuplicatedFreePool)
		{
			LocalPoolActor->Destroy();
		}
		DuplicatedFreePool.Empty();
	}
	XDupData = FDuplicationData(EMBAxis::AxisX,1,0.f,true);
	YDupData = FDuplicationData(EMBAxis::AxisY,1,0.f,true);
	ZDupData = FDuplicationData(EMBAxis::AxisZ,1,0.f,true);
	DuplicationFilters = FDuplicationFilters();
	SelectedDupActor = nullptr;
	Placeholders.Empty();
	ToolSettings->bIsDuplicationInprogress = false;
	bIsDuplicatingNow = false;
}

void UMBBuildingManager::ResetModularDuplication()
{
	XDupData = FDuplicationData(EMBAxis::AxisX,1,0.f,true);
	YDupData = FDuplicationData(EMBAxis::AxisY,1,0.f,true);
	ZDupData = FDuplicationData(EMBAxis::AxisZ,1,0.f,true);
	DuplicationFilters = FDuplicationFilters(false,false);
	
	MoveAllObjectToThePool();
}

bool UMBBuildingManager::FilterXDuplicationForHole(const uint16& InIndex,const uint16& InNum) const
{
	if(!DuplicationFilters.Hole){return true;}
	
	if(InNum < 3){return false;}
	
	if(InIndex == 0 || InIndex == (InNum - 1)) {return true;}

	return false;
}

bool UMBBuildingManager::FilterYDuplicationForHole(const uint16& InIndex, const uint16& InNum) const
{
	if(!DuplicationFilters.Hole){return true;}
	if(InIndex == (InNum - 1)) {return true;}
	return false;
}

bool UMBBuildingManager::FilterDuplicationForRectangle() const
{
	if(!DuplicationFilters.Rectangle){return false;}
	if(XDupData.NumOfDup < 1 || YDupData.NumOfDup < 1){return false;}
	return true;
}

#pragma endregion Duplication

#pragma region MultiplePlacement

void UMBBuildingManager::PlaceMultiple()
{
	if(!GEditor){return;}
	
	if(ToolSettings->MovedAssetCategory == EBuildingCategory::Modular && PlacementStatus == EPlacementStatus::Placeable && ToolSettings->PlacementMode != EPlacementMode::MultiModular)
	{
		TargetLocation = FVector(FMath::GridSnap(TargetLocation.X,1.0f),FMath::GridSnap(TargetLocation.Y,1.0f),FMath::GridSnap(TargetLocation.Z,1.0f));
		MovedActor->SetActorLocation(TargetLocation);
		
		ToolSettings->PlacementMode = EPlacementMode::MultiModular;
	}
	else if(ToolSettings->PlacementMode == EPlacementMode::MultiModular)
	{
		if(ToolSettings->GetToolMainScreen().Get())
		{
			Cast<IMBMainScreenInterface>(ToolSettings->GetToolMainScreen().Get())->SetMultiplePlacementAmountText(FString());
		}
		
		ToolSettings->PlacementMode = EPlacementMode::None;

		if(MultiPlacementBaseComponent->IsValidInstance(0))
		{
			const FText TransactionDescription = FText::FromString(TEXT("Multiplace Placement"));
			const auto TransactionIndex = GEngine->BeginTransaction(TEXT("MultiplacePlacement"), TransactionDescription, nullptr);
			
			const int32 Num = MultiPlacementBaseComponent->GetInstanceCount();
			
			const auto FirstClone = CreateModActor(GetMMAssetName(MovedActor),MovedActor->GetActorLocation(),MovedActor->GetActorRotation(),ToolSettings->MovedAssetCategory);

			if(!FirstClone)
			{
				GEngine->CancelTransaction(TransactionIndex);
				return;
			}
			
			if(Num > 0)
			{
				
				if(UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
				{
					//Create Constantly Used Variables Here
					FTransform Transform;
					
					//Replace All Instances
					for(int32 i = 0; i < Num ; i++)
					{
						if(const auto LocalRef = EditorActorSubsystem->DuplicateActor(FirstClone,GEditor->GetEditorWorldContext().World()))
						{
							MultiPlacementBaseComponent->GetInstanceTransform(i,Transform,true);
							LocalRef->SetActorLocationAndRotation(Transform.GetLocation(),Transform.GetRotation(),false);
							CheckForAnotherModularObjectInSameSpot(LocalRef);
						}
					}
				}
			}
			GEngine->EndTransaction();
			
			MultiPlacementBaseComponent->ClearInstances();
		}
		//Copy Asset Name
		FString MovedAssetName;
		if(MovedActor)
		{
			MovedAssetName = GetMMAssetName(MovedActor);	
		}
		
		DestroyMovedActor();
		ToolSettings->PlacementMode = EPlacementMode::None;
		ToolSettings->MovedAssetCategory = EBuildingCategory::None;
		ToolSettings->WorkingMode = EMBWorkingMode::None;
		
		//Recreate Asset
		CreateAssetAndStartPlacementProgress(MovedAssetName);
	}
}

void UMBBuildingManager::CheckForAnotherModularObjectInSameSpot(AActor* InActor) const
{
	const FVector Position = GetMMWorldOrigin(InActor);
	FVector Extent = GetMMLocalExtent(InActor);
	Extent = Extent / 1.5f;
	
	FHitResult HitResult;
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(MovedActor);
	ActorToIgnore.Add(InActor);
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	UKismetSystemLibrary::BoxTraceSingle(GEditor->GetEditorWorldContext().World(),Position,Position,Extent,InActor->GetActorRotation(),TraceType,true,ActorToIgnore,EDrawDebugTrace::None,HitResult,false);
	
	if(HitResult.bBlockingHit)
	{
		if(HitResult.GetActor() && UMBActorFunctions::IsActorFromTool(HitResult.GetActor()))
		{
			if(GetModularType(MovedActor).IsEqual(GetModularType(HitResult.GetActor())))
			{
				HitResult.GetActor()->Modify();
				HitResult.GetActor()->Destroy();
			}
		}
	}
}


void UMBBuildingManager::CancelMultiPlacement()
{
	if(MultiPlacementBaseComponent)
	{
		MultiPlacementBaseComponent->ClearInstances();
	}
	
	if(ToolSettings && ToolSettings->GetToolMainScreen().Get())
	{
		Cast<IMBMainScreenInterface>(ToolSettings->GetToolMainScreen().Get())->SetMultiplePlacementAmountText(FString());
	}
	
	DestroyMovedActor();
	bPlacementInProgress = false;
	bThirdAxisPositionState = false;
	bRepositionMainAxis = false;
	ToolSettings->PlacementMode = EPlacementMode::None;
	ToolSettings->MovedAssetCategory = EBuildingCategory::None;
	ToolSettings->WorkingMode = EMBWorkingMode::None;
}

void UMBBuildingManager::UpdateMultiPlacement() const
{
	FMultiPlacementData FirstAxisData;
	FMultiPlacementData SecondAxisData;
	
	CalculateFirstAndSecondMultiPlacementAxis(FirstAxisData.Axis,SecondAxisData.Axis);
	const auto ThirdAxisVec = UMBExtendedMath::GetAxisVectorWithEnumValue(UMBExtendedMath::GetSecAxis(FirstAxisData.Axis,SecondAxisData.Axis));

	//Calculate First and  SecondAxis
	const FVector MovedOrigin = GetMMWorldOrigin(MovedActor);
	const FVector LineOrigin = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetOrigin();
	const FVector SafeDir = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection().GetSafeNormal();
	const FVector LineEnd = LineOrigin + SafeDir * ToolSettings->GetToolUserSettings()->ObjectTracingDistance;
	
	const FPlane Plane = UKismetMathLibrary::MakePlaneFromPointAndNormal(MovedOrigin, ThirdAxisVec);

	FVector TargetPoint;

	if(FMath::SegmentPlaneIntersection(LineOrigin, LineEnd, Plane, TargetPoint))
	{
		TraceToSurface(LineOrigin,LineEnd,TargetPoint);
		
		CalculateMultiActorCountForFirstAxis(FirstAxisData.Axis,FirstAxisData.Direction,FirstAxisData.SpawnCount,FirstAxisData.HalfLength,TargetPoint);
		CalculateMultiActorCountForSecondAxis(SecondAxisData.Axis,SecondAxisData.Direction,SecondAxisData.SpawnCount,SecondAxisData.HalfLength,TargetPoint);

		//Include Moved Actor for calculation then remove it
		const int32 FinalSpawnCount = CalculateTotalMultiSpawnCount(FirstAxisData.SpawnCount,SecondAxisData.SpawnCount);
		CreateMultiPlacementActorNeed(FinalSpawnCount);
		PlaceMultipleObjects(FirstAxisData,SecondAxisData);

		if(ToolSettings->GetToolMainScreen().Get())
		{
			const int32 XValue = FMath::Clamp(FirstAxisData.SpawnCount + 1,1,INT32_MAX);
			const int32 YValue = FMath::Clamp(SecondAxisData.SpawnCount + 1,1,INT32_MAX);
			
			FString TotalValueStr;
			if(XValue *  YValue > 1)
			{
				TotalValueStr = FString::Printf(TEXT("  |  %d Objects"),XValue *  YValue);
			}
			
			
			const FString AmountText = FString::Printf(TEXT("%d X %d%s"),XValue,YValue,*TotalValueStr);
			Cast<IMBMainScreenInterface>(ToolSettings->GetToolMainScreen().Get())->SetMultiplePlacementAmountText(AmountText);
		}
	}
}

void UMBBuildingManager::TraceToSurface(const FVector& InTraceOrigin,const FVector& InTraceEnd, FVector& InTargetLocation) const 
{
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	FHitResult HitResult;

	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams CollisionParams(lineTraceSingleName);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;
	CollisionParams.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	CollisionParams.AddIgnoredActor(MovedActor);
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Visibility);
	
	EditorWorld->LineTraceSingleByObjectType(HitResult, InTraceOrigin, InTraceEnd, ObjectQueryParams, CollisionParams);
	if(HitResult.bBlockingHit)
	{
		if(HitResult.GetActor() && !UMBActorFunctions::IsActorFromTool(HitResult.GetActor()))
		{
			InTargetLocation.Z = HitResult.Location.Z;
		}
	}
}



void UMBBuildingManager::CalculateMultiActorCountForFirstAxis(const EMBAxis& InAxis,FVector& Direction,int32& OutCount,float& OutHalfLength,const FVector& TargetPoint) const
{
	if(!MovedActor){return;}

	//Calculate Mesh Length
	
	const float HalfLength = FMath::Abs(GetMMHalfLengthOfAxis(MovedActor,InAxis));
	OutHalfLength = HalfLength;
	if(HalfLength < 50.0f) {return;}

	//Calculate Direction
	const float DirSign = (CalculateDirectionOfAxis(InAxis) ? 1.0f : -1.0f);
	Direction = UMBExtendedMath::GetAxisVectorWithEnumValue(InAxis) * DirSign;
	
	float DistToTargetPoint = FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,TargetPoint) - UMBExtendedMath::GetAxisOfVector(InAxis,MovedActor->GetActorLocation()));
	
	if(GetMMMeshAxisStatus(MovedActor,InAxis) == (DirSign > 0.0f)) //Change
	{
		DistToTargetPoint -= HalfLength * 2;
	}
	
	float Offset = 0;
	if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->BuildingSettingsData.bSnapOffsetCond)
	{
		Offset = ToolSettings->GetToolData()->BuildingSettingsData.SnapOffset;
	}
	
	OutCount =  FMath::RoundToInt(DistToTargetPoint / ((HalfLength * 2) + Offset));
}

void UMBBuildingManager::CalculateMultiActorCountForSecondAxis(const EMBAxis& InAxis,FVector& Direction,int32& OutCount,float& OutHalfLength,const FVector& TargetPoint) const
{
	if(!MovedActor) {return;}
	
	//Calculate Mesh Length
	const float HalfLength = FMath::Abs(GetMMHalfLengthOfAxis(MovedActor,InAxis));
	OutHalfLength = HalfLength;
	if(HalfLength < 20.0f) {return;}

	//Calculate Direction
	const float DirSign = (CalculateDirectionOfAxis(InAxis) ? 1.0f : -1.0f);
	Direction = UMBExtendedMath::GetAxisVectorWithEnumValue(InAxis) * DirSign;


	float Offset = 0;
	if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->BuildingSettingsData.bSnapOffsetCond)
	{
		Offset = ToolSettings->GetToolData()->BuildingSettingsData.SnapOffset;
	}
	
	float DistToTargetPoint = FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,TargetPoint) - UMBExtendedMath::GetAxisOfVector(InAxis,MovedActor->GetActorLocation()));
	
	if(GetMMMeshAxisStatus(MovedActor,InAxis) == (DirSign > 0.0f)) //Change
	{
		DistToTargetPoint -= HalfLength * 2;
	}
	
	OutCount =  FMath::RoundToInt(DistToTargetPoint / ((HalfLength * 2) + Offset));
}

int32 UMBBuildingManager::CalculateTotalMultiSpawnCount(const int32& InFirstCount, const int32& InSecondCount)
{
	if(InSecondCount <= 0)
	{
		return InFirstCount;
	}
	return (InFirstCount + 1) * (InSecondCount + 1) - 1;
}

void UMBBuildingManager::CreateMultiPlacementActorNeed(int32 InCount) const
{
	if(InCount < 0){return;}

	const int32 MultiSpawnedNum = MultiPlacementBaseComponent->GetInstanceCount();
	
	if(MultiSpawnedNum < InCount) //Spawn 
	{
		const int32 ActorCountToSpawn = InCount - MultiSpawnedNum;

		FTransform Transform;
		Transform.SetRotation(MovedActor->GetActorRotation().Quaternion());
		
		for(int32 i = 0; i < ActorCountToSpawn; i++)
		{
			MultiPlacementBaseComponent->AddInstance(Transform,true);
		}
	}
	else if(MultiSpawnedNum > InCount && MultiSpawnedNum > 1) //Move Extra Actors To The Pool
	{
		if(MultiSpawnedNum > 0)
		{
			const int32 ExtraCount = MultiSpawnedNum - InCount;
			for(int32 Index = 0 ; Index < ExtraCount ; Index++)
			{
				MultiPlacementBaseComponent->RemoveInstance(MultiPlacementBaseComponent->GetInstanceCount()-1);
			}
		}
	}
}

void UMBBuildingManager::PlaceMultipleObjects(const FMultiPlacementData& InFirstMultiData,const FMultiPlacementData& InSecondMultiData) const
{
	if(!GEditor){return;}
	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSettingsSubsystem){return;}
	
	int32 TotalIndex = 0;
	
	FTransform Transform;
	Transform.SetRotation(MovedActor->GetActorRotation().Quaternion());

	float Offset = 0.0f;
	if(ToolSettings && ToolSettings->GetToolData() && ToolSettings->GetToolData()->BuildingSettingsData.bSnapOffsetCond)
	{
		Offset = ToolSettings->GetToolData()->BuildingSettingsData.SnapOffset;
	}
	
	const auto FirstSpawnCount = InFirstMultiData.SpawnCount;
	for(int32 FirstIndex = 0; FirstIndex < FirstSpawnCount ; FirstIndex++)
	{
		if(TotalIndex >= MultiPlacementBaseComponent->GetInstanceCount()){break;}

		FVector FirstTargetLoc = (InFirstMultiData.Direction  * ((FirstIndex + 1) * 2 * InFirstMultiData.HalfLength)) + MovedActor->GetActorLocation();
		
		FirstTargetLoc += InFirstMultiData.Direction * ((FirstIndex +1) * Offset);
		
		const int32 RefFirstIndex = TotalIndex;
		if(MultiPlacementBaseComponent->IsValidInstance(TotalIndex))
		{
			Transform.SetLocation(FirstTargetLoc);
			MultiPlacementBaseComponent->UpdateInstanceTransform(TotalIndex,Transform,true);
			++TotalIndex;
		}
		
		if(InSecondMultiData.SpawnCount > 0)
		{
			for(int32 SecIndex = 0; SecIndex < InSecondMultiData.SpawnCount; SecIndex++)
			{
				if(TotalIndex >= MultiPlacementBaseComponent->GetInstanceCount()){break;}

				FTransform LocTransform;
				MultiPlacementBaseComponent->GetInstanceTransform(RefFirstIndex,LocTransform,true);
				
				FVector SecTargetLoc = (InSecondMultiData.Direction  * ((SecIndex + 1) * 2 * InSecondMultiData.HalfLength)) + LocTransform.GetLocation();
				SecTargetLoc +=  InSecondMultiData.Direction * ((SecIndex +1) * Offset);

				if(MultiPlacementBaseComponent->IsValidInstance(TotalIndex))
				{
					Transform.SetLocation(SecTargetLoc);
					MultiPlacementBaseComponent->UpdateInstanceTransform(TotalIndex,Transform,true);
					++TotalIndex;
				}
			}
		}
	}
	
	if(InSecondMultiData.SpawnCount > 0)
	{
		for(int32 SecIndex = 0; SecIndex < InSecondMultiData.SpawnCount; SecIndex++)
		{
			if(TotalIndex >= MultiPlacementBaseComponent->GetInstanceCount()){break;}
				
			FVector SecTargetLoc = (InSecondMultiData.Direction  * ((SecIndex + 1) * 2 * InSecondMultiData.HalfLength)) + MovedActor->GetActorLocation();
			SecTargetLoc += InSecondMultiData.Direction * ((SecIndex +1) * Offset);

			if(MultiPlacementBaseComponent->IsValidInstance(TotalIndex))
			{
				Transform.SetLocation(SecTargetLoc);
				MultiPlacementBaseComponent->UpdateInstanceTransform(TotalIndex,Transform,true);
				++TotalIndex;
			}
		}
	}
}

void UMBBuildingManager::GetNearestHitAxisAndDirection(FVector& OutDirection, EMBAxis& OutAxis) const
{
	if(!MovedActor){return;}
	
	EMBAxis BlockedMultiAxis;
	FVector BlockedDirection;

	UMBExtendedMath::GetNearestAxisOfVector(GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection(),BlockedMultiAxis,BlockedDirection);
	
	if(GlobalHitResult.bBlockingHit)
	{
		FVector NormalizedDir = (GlobalHitResult.Location - MovedActor->GetActorLocation());
		NormalizedDir.Normalize();

		const float MaxAbs = NormalizedDir.GetAbs().GetMax();
		if(FMath::IsNearlyEqual(MaxAbs, NormalizedDir.GetAbs().X,0.001f))
		{
			OutDirection = FVector::XAxisVector * FMath::Sign(NormalizedDir.X);
			OutAxis = EMBAxis::AxisX;
		}
		else if(FMath::IsNearlyEqual(MaxAbs, NormalizedDir.GetAbs().Y,0.001f))
		{
			OutDirection = FVector::YAxisVector * FMath::Sign(NormalizedDir.Y);
			OutAxis = EMBAxis::AxisY;
		}
		else
		{
			OutDirection = FVector::ZAxisVector * FMath::Sign(NormalizedDir.Z);;
			OutAxis = EMBAxis::AxisZ;
		}	}
	else
	{
		const FVector LineOrigin = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetOrigin();
		
		//Find NearestPointOnLine
		const FVector SafeDir = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection().GetSafeNormal();
		
		const FVector ClosestPoint = LineOrigin + (SafeDir * (( GetMMWorldOrigin(MovedActor) -LineOrigin) | SafeDir));
		
		OutDirection = FMath::Sign(ClosestPoint.Z) * FVector::ZAxisVector;
		OutAxis = EMBAxis::AxisZ;
	}
}

void UMBBuildingManager::CalculateFirstAndSecondMultiPlacementAxis(EMBAxis& OutFirstAxis,EMBAxis& OutSecondAxis)
{
	EMBAxis BlockedMultiAxis;
	const FVector InVector = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection();
	
	const float MaxAbs = InVector.GetAbs().GetMax();
	if(FMath::IsNearlyEqual(MaxAbs, InVector.GetAbs().X,0.001f))
	{
		BlockedMultiAxis = EMBAxis::AxisX;
	}
	else if(FMath::IsNearlyEqual(MaxAbs, InVector.GetAbs().Y,0.001f))
	{
		BlockedMultiAxis = EMBAxis::AxisY;
	}
	else
	{
		BlockedMultiAxis = EMBAxis::AxisZ;
	}
	
	if(BlockedMultiAxis == EMBAxis::AxisX)
	{
		OutFirstAxis = EMBAxis::AxisY;
		OutSecondAxis = EMBAxis::AxisZ;
	}
	else if(BlockedMultiAxis == EMBAxis::AxisY)
	{
		OutFirstAxis = EMBAxis::AxisX;
		OutSecondAxis = EMBAxis::AxisZ;
	}
	else
	{
		OutFirstAxis = EMBAxis::AxisX;
		OutSecondAxis = EMBAxis::AxisY;
	}
}

bool UMBBuildingManager::CalculateDirectionOfAxis(const EMBAxis& InFirstAxis) const
{
	float Sign;
	FVector NormalizedDir;
	if(GlobalHitResult.bBlockingHit)
	{
		NormalizedDir = (GlobalHitResult.Location - MovedActor->GetActorLocation());
		NormalizedDir.Normalize();
	}
	else
	{
		const FVector LineOrigin = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetOrigin();
		const FVector SafeDir = GCurrentLevelEditingViewportClient->GetCursorWorldLocationFromMousePos().GetDirection().GetSafeNormal();
		const FVector Dir = (SafeDir * 10000) + LineOrigin;

		NormalizedDir = (Dir - MovedActor->GetActorLocation());
		NormalizedDir.Normalize();
	}
	if(InFirstAxis == EMBAxis::AxisX)
	{
		Sign =  FMath::Sign(NormalizedDir.X);
	}
	else if (InFirstAxis == EMBAxis::AxisY)
	{
		Sign =  FMath::Sign(NormalizedDir.Y);
	}
	else
	{
		Sign =  FMath::Sign(NormalizedDir.Z);
	}
	return Sign > 0 ? true : false;
}



#pragma endregion MultiplePlacement

#pragma region ModularMeshOperations

UStaticMeshComponent* UMBBuildingManager::GetMMComponent(const AActor* InActor) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		return Cast<AStaticMeshActor>(InActor)->GetStaticMeshComponent();
	}
	return nullptr;
}

FVector UMBBuildingManager::GetFixedMMLocation(const AActor* InActor) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			const FBox CollisionBox = MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBodySetup()->AggGeom.CalcAABB(FTransform::Identity);
			return MMActor->GetActorTransform().TransformPosition(CollisionBox.GetClosestPointTo(FVector::ZeroVector));
		}
	}
	return FVector::ZeroVector;
}

FString UMBBuildingManager::GetMMAssetName(const AActor* InActor) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			return MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetName();

		}
	}
	return FString();
}

FVector UMBBuildingManager::GetMMLocalExtent(const AActor* InActor) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			return MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBounds().BoxExtent;
		}
	}
	return FVector::ZeroVector;
}

FVector UMBBuildingManager::GetMMWorldExtent(const AActor* InActor) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			return MMActor->GetActorTransform().TransformPosition(MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBounds().BoxExtent); //Component Bounds

		}
	}
	return FVector::ZeroVector;
}

FVector UMBBuildingManager::GetMMWorldOrigin(const AActor* InActor) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			return MMActor->GetActorTransform().TransformPosition(MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBounds().Origin); //Component Bounds
		}
	}
	return FVector::ZeroVector;
}

bool UMBBuildingManager::GetMMMeshAxisStatus(const AActor* InActor,const EMBAxis InAxis) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			return UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetStaticMeshComponent()->Bounds.Origin) - UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetActorLocation()) > 0;
		}
	}
	return false;
}

bool UMBBuildingManager::IsMMMeshOnCenterOfAxis(const AActor* InActor,const EMBAxis& InAxis) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			const float Extent =  FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetStaticMeshComponent()->Bounds.BoxExtent)); // Extent
			const float PivotLoc = UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetActorLocation());
			const float BoundLoc = UMBExtendedMath::GetAxisOfVector(InAxis,GetMMWorldOrigin(InActor));
			return FMath::IsNearlyEqual(PivotLoc,BoundLoc,Extent * 0.02);
		}
	}
	return false;
}

float UMBBuildingManager::GetMMMeshLengthInDirectionAndAxis(const AActor* InActor,const EMBAxis& InAxis, const bool& InWorkingDir) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			if(!GEditor){return float();}
			const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
			if(!ToolSettingsSubsystem){return 0.0f;}

			float OriginOfAxis;
			float PivotAxis;
			float Extent;

			
			//Calculate Fixed Locations
			if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.SnappingType == EMBModularSnapType::SMCollision)
			{
				const FBox CollisionBox = MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBodySetup()->AggGeom.CalcAABB(FTransform::Identity);
				Extent =  UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetActorRotation().RotateVector(CollisionBox.GetExtent()));
				OriginOfAxis = UMBExtendedMath::GetAxisOfVector(InAxis,GetMMWorldOrigin(MMActor));
				PivotAxis = UMBExtendedMath::GetAxisOfVector(InAxis,GetFixedMMLocation(MMActor));
			}
			else
			{
				Extent =  UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetStaticMeshComponent()->Bounds.BoxExtent); // Extent
				OriginOfAxis = UMBExtendedMath::GetAxisOfVector(InAxis,GetMMWorldOrigin(MMActor)); //Origin
				PivotAxis = UMBExtendedMath::GetAxisOfVector(InAxis,GetFixedMMLocation(MMActor));
			}

			//Fix Extent With Scale
			const float ScaleRate = UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetStaticMeshComponent()->GetComponentTransform().GetScale3D());
			Extent *= ScaleRate;
			
			Extent = FMath::Abs(Extent);
	
			if(InWorkingDir)
			{
				OriginOfAxis += Extent;
			}
			else
			{
				OriginOfAxis -= Extent;
			}

			float Result = FMath::Abs( OriginOfAxis - PivotAxis);

			if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.SnappingType == EMBModularSnapType::BoundCorrection)
			{
				Result = FMath::GridSnap(Result,ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.BoundCorrectionSensitivity);	
			}
			return Result;
		}
	}
	return 0;
}

float UMBBuildingManager::GetMMHalfLengthOfAxis(const AActor* InActor,const EMBAxis& InAxis) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			if(!GEditor){return float();}
			const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
			if(!ToolSettingsSubsystem){return 0.0f;}

			float Extent;
	
			if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.SnappingType == EMBModularSnapType::SMCollision)
			{
				const FBox CollisionBox = MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBodySetup()->AggGeom.CalcAABB(FTransform::Identity);
				Extent =  FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetActorRotation().RotateVector(CollisionBox.GetExtent())));
			}
			else
			{
				Extent =  FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetStaticMeshComponent()->Bounds.BoxExtent)); // Extent
			}
			
			if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.SnappingType == EMBModularSnapType::BoundCorrection)
			{
				Extent = FMath::GridSnap(Extent,ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.BoundCorrectionSensitivity / 2.0f);	
			}

			return Extent;
			//return FMath::FloorToFloat(Extent);
		}
	}
	return 0;
}



#pragma endregion ModularMeshOperations

#pragma region ShuttingDown

void UMBBuildingManager::BeginDestroy()
{
	if(ToolSettings)
	{
		ToolSettings->OnPlacementTypeChanged.RemoveAll(this);
	}
	
	UObject::BeginDestroy();
}

void UMBBuildingManager::ShutDownBuildingManager()
{
	bCanTick = false;
	
	if(bPlacementInProgress)
	{
		CancelPlacement();
	}

	if(ToolSettings)
	{
		ToolSettings->OnPlacementTypeChanged.RemoveAll(this);
	}
}

#pragma endregion ShuttingDown