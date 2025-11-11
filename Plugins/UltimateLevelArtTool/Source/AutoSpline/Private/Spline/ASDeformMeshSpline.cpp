// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Spline/ASDeformMeshSpline.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#if WITH_EDITOR //Editor Only Includes
#include "Framework/Docking/TabManager.h"
#endif


void AASDeformMeshSpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	RedesignTheSplineElements();
}

void AASDeformMeshSpline::RedesignTheSplineElements()
{
	Super::RedesignTheSplineElements();

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
	
	const int32 FormerCompNum = SplineMeshComponents.Num();
	if(FormerCompNum > 0)
	{
		for(int32 Index = 0 ; Index < FormerCompNum ; Index++)
		{
			if(SplineMeshComponents.IsValidIndex(FormerCompNum - Index - 1))
			{
				if(SplineMeshComponents[FormerCompNum - Index - 1])
				{
					SplineMeshComponents[FormerCompNum - Index - 1]->DestroyComponent();
				}
				SplineMeshComponents.RemoveAt(FormerCompNum - Index - 1);
			}
		}
		SplineMeshComponents.Empty();
	}
	
#if WITH_EDITOR
	
	CheckForClosedStatus();

#endif
	

	UKismetMathLibrary::SetRandomStreamSeed(RandomStream,Seed);
	
	if(StaticMeshes.IsEmpty()){return;}

	//Remove EmptyMeshes
	TArray<TObjectPtr<UStaticMesh>> FilteredMeshes = StaticMeshes; 
	const int32 SMNum = FilteredMeshes.Num();
	
	if(FilteredMeshes.IsEmpty()){return;}

	for(int32 Index = 0 ; Index < SMNum ; Index++)
	{
		if(!FilteredMeshes[SMNum - Index - 1]){FilteredMeshes.RemoveAt(SMNum - Index - 1);}
	}
	
	if(FilteredMeshes.IsEmpty()){return;}

	const int32 MeshVariety = FilteredMeshes.Num();
	int32 TotalIndex = 0; 
	int32 CurrentMeshIndex = bSelectRandom ? UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,MeshVariety - 1) : 0;
	float TotalUsedLength = 0;
	while (true)
	{
		const float MeshLength = GetMeshLength(FilteredMeshes[CurrentMeshIndex],ForwardAxis) * ForwardScale;
		
		if(MeshLength + TotalUsedLength > GetSplineComp()->GetSplineLength()){break;}
		
		if(const auto CreatedSplineMeshComp = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(),true,FTransform(),false)))
		{
			CreatedSplineMeshComp->SetWorldTransform(GetSplineComp()->GetComponentTransform());
			
			CreatedSplineMeshComp->SetSplineUpDir(GetSplineUpVector(),false);
			
			CreatedSplineMeshComp->SetStaticMesh(FilteredMeshes[CurrentMeshIndex]);
			
			if(ForwardAxis == EASMeshAxis::AxisX)
			{
				CreatedSplineMeshComp->SetForwardAxis(ESplineMeshAxis::X);
			}
			else if(ForwardAxis == EASMeshAxis::AxisY)
			{
				CreatedSplineMeshComp->SetForwardAxis(ESplineMeshAxis::Y);
			}
			else
			{
				CreatedSplineMeshComp->SetForwardAxis(ESplineMeshAxis::Z);
			}
			
			CreatedSplineMeshComp->SetStartScale(FVector2D(GetSplineComp()->GetScaleAtDistanceAlongSpline(TotalUsedLength).X,GetSplineComp()->GetScaleAtDistanceAlongSpline(TotalUsedLength).Y),true);
			CreatedSplineMeshComp->SetEndScale(FVector2D(GetSplineComp()->GetScaleAtDistanceAlongSpline(TotalUsedLength + MeshLength).X,GetSplineComp()->GetScaleAtDistanceAlongSpline(TotalUsedLength).Y),true);
			CreatedSplineMeshComp->SetStartAndEnd(
			GetSplineComp()->GetLocationAtDistanceAlongSpline(TotalUsedLength,ESplineCoordinateSpace::Type::Local), //Start Pos
			GetSplineComp()->GetTangentAtDistanceAlongSpline(TotalUsedLength,ESplineCoordinateSpace::Type::Local).GetClampedToSize(MeshLength,MeshLength), //Start Tangent
			GetSplineComp()->GetLocationAtDistanceAlongSpline(TotalUsedLength + MeshLength,ESplineCoordinateSpace::Type::Local),//End Pos
			GetSplineComp()->GetTangentAtDistanceAlongSpline(TotalUsedLength + MeshLength,ESplineCoordinateSpace::Type::Local).GetClampedToSize(MeshLength,MeshLength),true); //End Tangent
			
			SplineMeshComponents.Add(CreatedSplineMeshComp);
			
			TotalUsedLength += MeshLength + OffsetOnSpline;

			if(bSelectRandom)
			{
				CurrentMeshIndex = UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,MeshVariety - 1);	
			}
			else
			{
				if(++CurrentMeshIndex == MeshVariety)
				{
					CurrentMeshIndex = 0;
				}
			}
			++TotalIndex;
		}
	}
	
	if(SplineMeshComponents.IsEmpty()){return;}

	for(auto CurrentSmCom : SplineMeshComponents)
	{
		if(CurrentSmCom){CurrentSmCom->SetCollisionEnabled(CollisionEnabled);}
	}
}

FVector AASDeformMeshSpline::GetSplineUpVector() const
{
	if(ForwardAxis == EASMeshAxis::AxisX)
	{
		return FVector(0,0,1.0f);
	}
	if(ForwardAxis == EASMeshAxis::AxisY)
	{
		return FVector(0,-1.0f,0.0f);
	}
	return FVector(-1.0f,0,0);
}

void AASDeformMeshSpline::CheckForSurfaceSnapping()
{
	if(!GetWorld() || !GetSplineComp() || GetSplineComp()->GetNumberOfSplinePoints() == 0){return;}

	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams collisionParams(lineTraceSingleName);
	collisionParams.bTraceComplex = false;
	collisionParams.bReturnPhysicalMaterial = false;
	collisionParams.AddIgnoredActor(this);
	FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	objectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FHitResult HitResult;

	int32 SplineNum = GetSplineComp()->GetNumberOfSplinePoints();

	for(int32 Index = 0; Index < SplineNum; Index++)
	{
		FVector SplinePointLocation = GetSplineComp()->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
		
		GetWorld()->LineTraceSingleByObjectType(HitResult, SplinePointLocation, SplinePointLocation + FVector(0,0,-15000.0f), objectParams, collisionParams);
		if(HitResult.bBlockingHit)
		{
			GetSplineComp()->SetLocationAtSplinePoint(Index, HitResult.ImpactPoint, ESplineCoordinateSpace::World,false);
		}
	}
	GetSplineComp()->UpdateSpline();

	RedesignTheSplineElements();
}

#if WITH_EDITOR

void AASDeformMeshSpline::CheckForClosedStatus() const
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

FReply AASDeformMeshSpline::OnRunMergePressed()
{
	CreateActorsFromSpline();
	
	return FReply::Handled();
}

FReply AASDeformMeshSpline::OnSnapPointsToSurface()
{
	CheckForSurfaceSnapping();
	
	return FReply::Handled();
}

void AASDeformMeshSpline::CreateActorsFromSpline(bool bInSelectAfter)
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("MergeActors"));
}

#endif

