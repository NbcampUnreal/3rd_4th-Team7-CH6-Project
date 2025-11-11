// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Spline/ASStaticMeshDistributionSpline.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

#if WITH_EDITOR //Editor Only Includes

#include "Framework/Docking/TabManager.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "LevelEditorViewport.h"

#endif 

void AASStaticMeshDistributionSpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RedesignTheSplineElements();
}



void AASStaticMeshDistributionSpline::BeginPlay()
{
	Super::BeginPlay();

	CheckForSurfaceSnapping();
}

void AASStaticMeshDistributionSpline::RedesignTheSplineElements()
{
#if WITH_EDITOR
	
	if(bCopyASpline)
	{
		if(CopyASpline)
		{
			if(const auto AutoSplineActor = Cast<AASAutoSplineBase>(CopyASpline))
			{
				AutoSplineActor->CopyTargetSpline(GetSplineComp());

				SetActorRotation(CopyASpline->GetActorRotation());
			}
			CopyASpline = nullptr;
		}
	}
#endif
	
	ResetFormerSpline();

	UKismetMathLibrary::SetRandomStreamSeed(RandomStream,Seed);

#if WITH_EDITOR
	
	CheckForClosedStatus();
	
#endif
	
	CreateSplineMesh();

	CheckForSurfaceSnapping();
}

UStaticMeshComponent* AASStaticMeshDistributionSpline::CreateStaticMeshComponent(UStaticMesh* InStaticMesh, const FTransform& InRelativeTransform)
{
	UStaticMeshComponent* CreatedSmComp = NewObject<UStaticMeshComponent>(this);
	if(!CreatedSmComp){return nullptr;}
	CreatedSmComp->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
	CreatedSmComp->bAutoRegister = true;
	CreatedSmComp->SetStaticMesh(InStaticMesh);
	CreatedSmComp->SetupAttachment(GetSplineComp());
	CreatedSmComp->SetWorldTransform(GetSplineComp()->GetComponentTransform());
	CreatedSmComp->SetRelativeTransform(InRelativeTransform);
	FinishAndRegisterComponent(CreatedSmComp);
	return CreatedSmComp;
}

UInstancedStaticMeshComponent* AASStaticMeshDistributionSpline::CreateInstancedStaticMeshComponent(const UClass* InISmClass,UStaticMesh* InStaticMesh)
{
	UInstancedStaticMeshComponent* CreatedIsmComp = NewObject<UInstancedStaticMeshComponent>(this,InISmClass);
	if(!CreatedIsmComp){return nullptr;}
	CreatedIsmComp->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
	CreatedIsmComp->bAutoRegister = true;
	CreatedIsmComp->SetStaticMesh(InStaticMesh);
	CreatedIsmComp->SetupAttachment(GetSplineComp());
	CreatedIsmComp->SetWorldTransform(GetSplineComp()->GetComponentTransform());
	FinishAndRegisterComponent(CreatedIsmComp);
	
	return CreatedIsmComp;
}

void AASStaticMeshDistributionSpline::ResetFormerSpline()
{
	if(!DynamicSMInstanceComponents.IsEmpty())
	{
		const auto Num = DynamicSMInstanceComponents.Num();

		for(int32 Index = 0 ; Index < Num ; Index++)
		{
			if(DynamicSMInstanceComponents[Num - Index - 1])
			{
				DynamicSMInstanceComponents[Num - Index - 1]->DestroyComponent();
			}
		}
		DynamicSMInstanceComponents.Empty();
	}
	
	if(!DynamicSMComponents.IsEmpty())
	{
		const auto Num = DynamicSMComponents.Num();

		for(int32 Index = 0 ; Index < Num ; Index++)
		{
			if(DynamicSMComponents[Num - Index - 1])
			{
				DynamicSMComponents[Num - Index - 1]->DestroyComponent();
			}
		}
		DynamicSMComponents.Empty();
	}

}

int32 AASStaticMeshDistributionSpline::GetMeshContainingInstancedSMCompIndex(UStaticMesh* InStaticMesh)
{
	const int32 Num = DynamicSMInstanceComponents.Num();
	if(Num == 0){return -1;}
	
	for(int32 Index = 0 ; Index < Num ; Index++)
	{
		if(DynamicSMInstanceComponents[Index]->GetStaticMesh() == InStaticMesh)
		{
			return Index;
		}
	}
	return -1;
}


void AASStaticMeshDistributionSpline::CreateSplineMesh()
{
	if(SplineStaticMeshes.IsEmpty()){return;}
	//Cancel if spline too short

	TArray<TObjectPtr<UStaticMesh>> FilteredMeshes = SplineStaticMeshes; 

	//Remove Empty Mesh Slots
	const int32 SMNum = FilteredMeshes.Num();
	for(int32 Index = 0 ; Index < SMNum ; Index++)
	{
		if(!FilteredMeshes[SMNum - Index - 1]){FilteredMeshes.RemoveAt(SMNum - Index - 1);}
	}
	if(FilteredMeshes.IsEmpty()){return;}
	
	//If we are building instanced static mesh or derived types
	if(InstanceType != ENonDeformInstanceType::Sm)
	{
		//Create ISM Components for per SM assigned by user
		for(auto SelectedMesh : FilteredMeshes)
		{
			const auto IsmClass = InstanceType == ENonDeformInstanceType::Ism ? UInstancedStaticMeshComponent::StaticClass() : UHierarchicalInstancedStaticMeshComponent::StaticClass();
			const auto CreatedIsmComp = CreateInstancedStaticMeshComponent(IsmClass,SelectedMesh);
			DynamicSMInstanceComponents.Add(CreatedIsmComp);
		}
		if(bAddCustomEndMeshCond && CustomEndMesh)
		{
			//Means different mesh from instance list and need to add new one
			if(GetMeshContainingInstancedSMCompIndex(CustomEndMesh) < 0)
			{
				const auto IsmClass = InstanceType == ENonDeformInstanceType::Ism ? UInstancedStaticMeshComponent::StaticClass() : UHierarchicalInstancedStaticMeshComponent::StaticClass();
				const auto CreatedIsmComp = CreateInstancedStaticMeshComponent(IsmClass,CustomEndMesh);
				DynamicSMInstanceComponents.Add(CreatedIsmComp);
			}
		}
	}
	const int32 ObjectVariety = FilteredMeshes.Num();
	
	int32 TotalIndex = 0;  
	int32 CurrentObjectIndex = bSelectRandom ? UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,ObjectVariety - 1) : 0;
	GetSplineComp()->SetSplinePointType(TotalIndex,ESplinePointType::Type::Curve,true);

	FTransform Transform;
	
	//Number Of Instances
	if(bSetNumber)
	{
		if(bRandomScaleRange)
		{
			Transform.SetScale3D(FVector::OneVector * GetRandomScale());
		}
		
		const float LastLength = GetMeshLength(FilteredMeshes[CurrentObjectIndex],ForwardAxis);
		const float UnitLength = (GetSplineComp()->GetSplineLength() - LastLength)/(NumberOfInstance - 1);
		
		for(int32 Index = 0 ; Index < NumberOfInstance ; Index++)
		{
			bool bAddingCustomEndMesh = bAddCustomEndMeshCond && CustomEndMesh && (Index == 0 || Index == (NumberOfInstance -1));

			float MeshLength; 
			if(bAddingCustomEndMesh)
			{
				MeshLength = GetMeshLength(CustomEndMesh,ForwardAxis);
			}
			else
			{
				MeshLength = GetMeshLength(FilteredMeshes[CurrentObjectIndex],ForwardAxis);
			}
			
			FVector LocationOnSpline = GetSplineComp()->GetLocationAtDistanceAlongSpline(Index * UnitLength,ESplineCoordinateSpace::Type::Local);

			if(bInnerSplineOffset)
			{
				FVector SplineTangent = GetSplineComp()->GetTangentAtDistanceAlongSpline(Index * UnitLength,ESplineCoordinateSpace::Type::Local);
				SplineTangent = SplineTangent.GetSafeNormal();
				SplineTangent *= UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,InnerSplineOffset.X,InnerSplineOffset.Y);
				SplineTangent *= FMath::RandBool() ? 1.0 : -1.0f;
				LocationOnSpline += SplineTangent;
			}
			
			FRotator ObjectRotation = FRotator::ZeroRotator;
			if(RotationType != ESplineObjectRotationType::Keep)
			{
				//Rotation Follows The Spline
				FVector DirToNextMesh = GetSplineComp()->GetLocationAtDistanceAlongSpline((Index * UnitLength) + MeshLength,ESplineCoordinateSpace::Type::Local) - LocationOnSpline;
				ObjectRotation = FRotationMatrix::MakeFromX(DirToNextMesh).Rotator();
			}
			
			if(bRandomPositionOffset)
			{
				AddRandomPositionOffset(LocationOnSpline);
			}

			ObjectRotation =  FRotator(FQuat(ObjectRotation)*FQuat(InitialRotationOffset));

			if(bRandomRotationOffset)
			{
				AddRandomRotationOffset(ObjectRotation);
			}
			
			Transform.SetRotation(ObjectRotation.Quaternion());


			Transform.SetLocation(LocationOnSpline);
			
			if(InstanceType == ENonDeformInstanceType::Sm)
			{
				//Custom end mesh implementation
				if(bAddingCustomEndMesh)
				{
					auto CreatedSMComp = CreateStaticMeshComponent(CustomEndMesh,Transform);
					DynamicSMComponents.Add(CreatedSMComp);
				}
				else
				{
					auto CreatedSMComp = CreateStaticMeshComponent(FilteredMeshes[CurrentObjectIndex],Transform);
					DynamicSMComponents.Add(CreatedSMComp);
				}
			}
			else
			{
				//Custom end mesh implementation
				if(bAddingCustomEndMesh)
				{
					int32 CustomISMCompIndex = GetMeshContainingInstancedSMCompIndex(CustomEndMesh);
					if(CustomISMCompIndex >= 0)
					{
						DynamicSMInstanceComponents[CustomISMCompIndex]->AddInstance(Transform,false);
					}
				}
				else
				{
					DynamicSMInstanceComponents[CurrentObjectIndex]->AddInstance(Transform,false);
				}
			}
			
			//Select Next Mesh
			if(!bAddingCustomEndMesh)
			{
				if(bSelectRandom)
				{
					CurrentObjectIndex = UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,ObjectVariety - 1);
				}
				else
				{
					if(++CurrentObjectIndex == ObjectVariety)
					{
						CurrentObjectIndex = 0;
					}
				}
			}
			++TotalIndex;
		}
	}
	else
	{
		//Place Along the Spline With Offset
		float TotalUsedLength = 0;
		while (true)
		{
			float MeshLength;
			bool bAddingCustomEndMesh = bAddCustomEndMeshCond && CustomEndMesh && (TotalIndex == 0 || GetMeshLength(CustomEndMesh,ForwardAxis) * 2 + TotalUsedLength > GetSplineComp()->GetSplineLength());

			//Custom end mesh implementation
			if(bAddingCustomEndMesh)
			{
				MeshLength = GetMeshLength(CustomEndMesh,ForwardAxis);
			}
			else
			{
				MeshLength = GetMeshLength(FilteredMeshes[CurrentObjectIndex],ForwardAxis);
			}

			if(bRandomScaleRange)
			{
				float RandomScaleRate = GetRandomScale();
				Transform.SetScale3D(FVector::OneVector * RandomScaleRate);
				MeshLength *= RandomScaleRate;
			}
			
			if(MeshLength + TotalUsedLength > GetSplineComp()->GetSplineLength()){break;}
			FVector LocationOnSpline = GetSplineComp()->GetLocationAtDistanceAlongSpline(TotalUsedLength,ESplineCoordinateSpace::Type::Local);
			
			FRotator ObjectRotation = FRotator::ZeroRotator;
			if(RotationType != ESplineObjectRotationType::Keep)
			{
				//Rotation Follows The Spline
				FVector DirToNextMesh = GetSplineComp()->GetLocationAtDistanceAlongSpline(TotalUsedLength + MeshLength,ESplineCoordinateSpace::Type::Local) - LocationOnSpline;
				ObjectRotation = FRotationMatrix::MakeFromX(DirToNextMesh).Rotator();
			}
			
			if(bInnerSplineOffset)
			{
				FVector SplineTangent = GetSplineComp()->GetRightVectorAtDistanceAlongSpline(TotalUsedLength,ESplineCoordinateSpace::Type::Local);
				SplineTangent = SplineTangent.GetSafeNormal();
				SplineTangent *= UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,InnerSplineOffset.X,InnerSplineOffset.Y);
				SplineTangent *= FMath::RandBool() ? 1.0 : -1.0f;
				LocationOnSpline += SplineTangent;
			}

			ObjectRotation =  FRotator(FQuat(ObjectRotation)*FQuat(InitialRotationOffset));
			
			if(bRandomPositionOffset)
			{
				AddRandomPositionOffset(LocationOnSpline);
			}

			if(bRandomRotationOffset)
			{
				AddRandomRotationOffset(ObjectRotation);
			}
			
			Transform.SetRotation(ObjectRotation.Quaternion());
			
			Transform.SetLocation(LocationOnSpline);

			if(InstanceType == ENonDeformInstanceType::Sm)
			{
				//Custom end mesh implementation
				if(bAddingCustomEndMesh)
				{
					auto CreatedSMComp = CreateStaticMeshComponent(CustomEndMesh,Transform);
					DynamicSMComponents.Add(CreatedSMComp);
				}
				else
				{
					auto CreatedSMComp = CreateStaticMeshComponent(FilteredMeshes[CurrentObjectIndex],Transform);
					DynamicSMComponents.Add(CreatedSMComp);
				}
			}
			else
			{
				if(bAddingCustomEndMesh)
				{
					int32 CustomISMCompIndex = GetMeshContainingInstancedSMCompIndex(CustomEndMesh);
					if(CustomISMCompIndex >= 0)
					{
						DynamicSMInstanceComponents[CustomISMCompIndex]->AddInstance(Transform,false);
					}
				}
				else
				{
					DynamicSMInstanceComponents[CurrentObjectIndex]->AddInstance(Transform,false); 
				}
			}
			
			const float RealOffset = FMath::Clamp(OffsetOnSpline,-1 * MeshLength / 2,OffsetOnSpline);
			TotalUsedLength += MeshLength + RealOffset; //Offset cant be under

			//Select Next Mesh
			if(!bAddingCustomEndMesh)
			{
				if(bSelectRandom)
				{
					CurrentObjectIndex = UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,ObjectVariety - 1);
				}
				else
				{
					if(++CurrentObjectIndex == ObjectVariety)
					{
						CurrentObjectIndex = 0;
					}
				}
			}
			++TotalIndex;
		}
	}

	if(InstanceType != ENonDeformInstanceType::Sm)
	{
		if(!DynamicSMInstanceComponents.IsEmpty())
		{
			for(auto CurrentISMComp : DynamicSMInstanceComponents)
			{
				CurrentISMComp->SetMobility(Mobility);
			}
		}
	}
	else
	{
		if(!DynamicSMComponents.IsEmpty())
		{
			for(auto CurrentSMComp : DynamicSMComponents)
			{
				CurrentSMComp->SetMobility(Mobility);
			}
		}
			
	}
}

void AASStaticMeshDistributionSpline::AddRandomPositionOffset(FVector& InVector) const
{
	InVector += FVector(
UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomPositionOffset.X * -1,RandomPositionOffset.X),	
UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomPositionOffset.Y * -1,RandomPositionOffset.Y),	
UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomPositionOffset.Z * -1,RandomPositionOffset.Z));
}

void AASStaticMeshDistributionSpline::AddRandomRotationOffset(FRotator& InRotator) const
{
	InRotator.Pitch += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationOffset.Pitch * -1,RandomRotationOffset.Pitch);
	InRotator.Yaw += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationOffset.Yaw * -1,RandomRotationOffset.Yaw);
	InRotator.Roll += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationOffset.Roll * -1,RandomRotationOffset.Roll);
}

float  AASStaticMeshDistributionSpline::GetRandomScale() const
{
	return  UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomScaleRange.X,RandomScaleRange.Y);
}

void AASStaticMeshDistributionSpline::CheckForSurfaceSnapping() const
{
	if(SnapToSurface)
	{
		UWorld* LocalWorld = GetWorld();
		if(!LocalWorld){return;}
		
		static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
		FCollisionQueryParams collisionParams(lineTraceSingleName);
		collisionParams.bTraceComplex = false;
		collisionParams.bReturnPhysicalMaterial = false;
		FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
		objectParams.AddObjectTypesToQuery(ECC_Visibility);
		objectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		FHitResult HitResult;

		if(InstanceType == ENonDeformInstanceType::Sm)
		{
			TArray<UActorComponent*> FoundMesComps;
			GetComponents(UStaticMeshComponent::StaticClass(),FoundMesComps);
			if(FoundMesComps.IsEmpty()){return;}

			for(const auto CurrentSMComp : FoundMesComps)
			{
				if(USceneComponent* LocalSmComp = Cast<USceneComponent>(CurrentSMComp))
				{
					FVector CompLoc = LocalSmComp->GetComponentLocation();

					LocalWorld->LineTraceSingleByObjectType(HitResult, CompLoc, CompLoc + FVector(0,0,-15000.0f), objectParams, collisionParams);
					if(HitResult.bBlockingHit)
					{
						LocalSmComp->SetWorldLocation(HitResult.Location);
					}
					
					if(RotationType == ESplineObjectRotationType::FollowSplineAndSurface)
                    {
                    	//Rotation Follows The Spline & Surface
                    	FVector ObjectWorldForward  = GetActorTransform().TransformRotation(LocalSmComp->GetComponentRotation().Quaternion()).Rotator().Vector();
                    	FRotator MakeRot = FRotationMatrix::MakeFromZX(HitResult.Normal,ObjectWorldForward).Rotator();
                    	FRotator ObjectRotation = GetActorTransform().InverseTransformRotation(MakeRot.Quaternion()).Rotator();
						LocalSmComp->SetWorldRotation(ObjectRotation);
                    }
				}
			}
		}
		else
		{
			TArray<UActorComponent*> FoundInstSMComps;
			GetComponents(UInstancedStaticMeshComponent::StaticClass(),FoundInstSMComps);
			if(FoundInstSMComps.IsEmpty()){return;}

			FTransform CompTransform;

			for(const auto CurrentIsmComp : FoundInstSMComps)
			{
				if(UInstancedStaticMeshComponent* LocalIsmComp = Cast<UInstancedStaticMeshComponent>(CurrentIsmComp))
				{
					int32 Num = LocalIsmComp->GetInstanceCount();

					for(int32 Index = 0; Index < Num ; ++Index)
					{
						LocalIsmComp->GetInstanceTransform(Index,CompTransform,true);
						
						LocalWorld->LineTraceSingleByObjectType(HitResult, CompTransform.GetLocation(), CompTransform.GetLocation() + FVector(0,0,-15000.0f), objectParams, collisionParams);
						if(HitResult.bBlockingHit)
						{
							
							CompTransform.SetLocation(HitResult.Location);
						}
					
						if(RotationType == ESplineObjectRotationType::FollowSplineAndSurface)
						{
							//Rotation Follows The Spline & Surface
							FRotator MakeRot = FRotationMatrix::MakeFromZX(HitResult.Normal,CompTransform.GetRotation().Vector()).Rotator();
							FRotator ObjectRotation = GetActorTransform().InverseTransformRotation(MakeRot.Quaternion()).Rotator();
							CompTransform.SetRotation(ObjectRotation.Quaternion());
						}
						
						LocalIsmComp->UpdateInstanceTransform(Index,CompTransform,true);
					}
				}
			}
		}
	}
}

#if WITH_EDITOR

void AASStaticMeshDistributionSpline::CheckForClosedStatus() const
{
	if(!bAutoCloseTheSpline)
	{
		GetSplineComp()->SetClosedLoop(false,true);
	}
	else
	{
		const float  Result = FVector::DistSquared(GetSplineComp()->GetLocationAtDistanceAlongSpline(0,ESplineCoordinateSpace::Local),GetSplineComp()->GetLocationAtDistanceAlongSpline(GetSplineComp()->GetSplineLength(),ESplineCoordinateSpace::Local));

		if(Result < AutoCloseTheSpline * AutoCloseTheSpline)
		{
			GetSplineComp()->SetClosedLoop(true);
		}
		else
		{
			GetSplineComp()->SetClosedLoop(false);
		}
	}
}

FReply AASStaticMeshDistributionSpline::OnSetFreePressed()
{
	const FText TransactionDescription = FText::FromString(TEXT("Instance Creation"));
	GEngine->BeginTransaction(TEXT("Instance Creation"), TransactionDescription, nullptr);
	
	CreateActorsFromSpline(true);

	GEngine->EndTransaction();

	return FReply::Handled();
}

FReply AASStaticMeshDistributionSpline::OnRunMergePressed()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("MergeActors"));
	
	return FReply::Handled();
}

void AASStaticMeshDistributionSpline::CreateActorsFromSpline(bool bInSelectAfter)
{
	if(InstanceType == ENonDeformInstanceType::Sm)
	{
		if(DynamicSMComponents.IsEmpty())
		{
			return;
		}
	}
	else
	{
		if(DynamicSMInstanceComponents.IsEmpty())
		{
			return;
		}
	}
	
	const auto SpawnedActor = SpawnAndGetAnActor(GetActorTransform(),FString(TEXT("Static Mesh Distribution Actor")),GEditor->GetEditorWorldContext().World());
	if(!SpawnedActor){return;}
	
	if(InstanceType == ENonDeformInstanceType::Sm)
	{
		bool bFirstIndex = true;
		
		for(const auto ISMComp : DynamicSMComponents)
		{
			UStaticMeshComponent* CreatedSmComp = NewObject<UStaticMeshComponent>(SpawnedActor);
			if(!CreatedSmComp){continue;}

			if(bFirstIndex)
			{
				SpawnedActor->SetActorTransform(ISMComp->GetComponentTransform());
			}
			CreatedSmComp->SetupAttachment(SpawnedActor->GetRootComponent());
			SpawnedActor->AddInstanceComponent(CreatedSmComp);
			SpawnedActor->AddOwnedComponent(CreatedSmComp);
			CreatedSmComp->RegisterComponent();
			CreatedSmComp->SetStaticMesh(ISMComp->GetStaticMesh());
			CreatedSmComp->SetWorldTransform(ISMComp->GetComponentTransform());
			CreatedSmComp->SetMobility(Mobility);
			CreatedSmComp->SetFlags(RF_Transactional);

			bFirstIndex = false;
		}
	}
	else
	{
		if(DynamicSMInstanceComponents[0])
		{
			FTransform Transform;
			DynamicSMInstanceComponents[0]->GetInstanceTransform(0,Transform,true);
			SpawnedActor->SetActorTransform(Transform);
		}
		
		for(const auto ISMComp : DynamicSMInstanceComponents)
		{
			UInstancedStaticMeshComponent* CreatedIsmComp = NewObject<UInstancedStaticMeshComponent>(SpawnedActor,ISMComp->GetClass());
			if(!CreatedIsmComp){continue;}
			
			CreatedIsmComp->SetupAttachment(SpawnedActor->GetRootComponent());
			SpawnedActor->AddInstanceComponent(CreatedIsmComp);
			SpawnedActor->AddOwnedComponent(CreatedIsmComp);
			CreatedIsmComp->RegisterComponent();
			CreatedIsmComp->SetStaticMesh(ISMComp->GetStaticMesh());
			CreatedIsmComp->SetWorldTransform(ISMComp->GetComponentTransform());
			CreatedIsmComp->SetMobility(Mobility);
			CreatedIsmComp->SetFlags(RF_Transactional);

			FTransform Transform;
			const int32 InstNum =  ISMComp->GetInstanceCount();
			for(int32 Index = 0; Index < InstNum ; Index++)
			{
				ISMComp->GetInstanceTransform(Index,Transform,true);
				CreatedIsmComp->AddInstance(Transform,true);
			}
		}
	}

	if(bInSelectAfter)
	{
		if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
		{
			EditorActorSubsystem->SelectNothing();
			EditorActorSubsystem->SetActorSelectionState(SpawnedActor,true);
		}
	}
}


AActor* AASStaticMeshDistributionSpline::SpawnAndGetAnActor(const FTransform& InTransform,const FString& InActorLabel, UWorld* InWorld) const
{
	if(!InWorld){return nullptr;}
	
	//Create Actor 
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	
	const auto SpawnedActor = InWorld->SpawnActor<AActor>(AStaticMeshActor::StaticClass(),InTransform,SpawnParams);
	if(!SpawnedActor){return nullptr;}

	USceneComponent* CreatedRootComponent = NewObject<USceneComponent>(SpawnedActor,FName(TEXT("RootComponent")));
	if(!CreatedRootComponent){return SpawnedActor;}
	
	CreatedRootComponent->bAutoRegister = true;
	SpawnedActor->SetRootComponent(CreatedRootComponent);
	SpawnedActor->AddInstanceComponent(CreatedRootComponent);
	SpawnedActor->AddOwnedComponent(CreatedRootComponent);
	CreatedRootComponent->SetFlags(RF_Transactional);
	
	SpawnedActor->SetActorLabel(InActorLabel);
	SpawnedActor->GetRootComponent()->SetMobility(Mobility);
	
	return SpawnedActor;
}

#endif



