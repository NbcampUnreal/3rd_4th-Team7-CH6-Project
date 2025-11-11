// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Spline/ASAutoSplineBase.h"
#include "Components/SplineComponent.h"
#include "Spline/ASSplineComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

AASAutoSplineBase::AASAutoSplineBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SplineBody"));
	RootComponent = SceneRoot;

	SplineComponent = CreateDefaultSubobject<UASSplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(RootComponent);


#if WITH_EDITORONLY_DATA
	
	SplineComponent->ScaleVisualizationWidth = 50;
	SplineComponent->bShouldVisualizeScale = true;

#endif
	
}

void AASAutoSplineBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	
	if(IsValid(SplineComponent))
	{
		SplineComponent->OnComponentChanged.Unbind();
		SplineComponent->OnComponentChanged.BindUObject(this,&AASAutoSplineBase::OnSplineUpdated);
	}
#endif
	
}

USplineComponent* AASAutoSplineBase::GetSplineComp() const
{
	return SplineComponent;
}

float AASAutoSplineBase::GetMeshLength(const UStaticMesh* InStaticMesh,EASMeshAxis InMeshAxis)
{
	if(!InStaticMesh) {return 0;}

	switch (InMeshAxis) {
		
	case EASMeshAxis::AxisX:
		return static_cast<float>(FMath::RoundToInt32(InStaticMesh->GetBoundingBox().Max.X - InStaticMesh->GetBoundingBox().Min.X)) * GetActorScale().X;
	case EASMeshAxis::AxisY:
		return static_cast<float>(FMath::RoundToInt32(InStaticMesh->GetBoundingBox().Max.Y - InStaticMesh->GetBoundingBox().Min.Y)) * GetActorScale().Y;
	case EASMeshAxis::AxisZ:
		return static_cast<float>(FMath::RoundToInt32(InStaticMesh->GetBoundingBox().Max.Z - InStaticMesh->GetBoundingBox().Min.Z)) * GetActorScale().Z;
	default:
		return static_cast<float>(FMath::RoundToInt32(InStaticMesh->GetBoundingBox().Max.X - InStaticMesh->GetBoundingBox().Min.X)) * GetActorScale().X;
	}
}

float AASAutoSplineBase::GetComponentLength(const USceneComponent* InSceneComponent)
{
	if(!InSceneComponent) {return 0;}
	return InSceneComponent->GetLocalBounds().BoxExtent.X * 2 * GetActorScale().X;
}

float AASAutoSplineBase::GetActorLength(const AActor* InActor)
{
	if(!InActor) {return 0;}
	FVector Origin;
	FVector BoxExtent;
	InActor->GetActorBounds(false,Origin,BoxExtent);
	return (BoxExtent.X + BoxExtent.Y + BoxExtent.Z) * 0.75 * GetActorScale().X;

}


void AASAutoSplineBase::AdjustSplineLabel()
{
#if WITH_EDITOR
	SetActorLabel(GetAsASplineLabel());
#endif
	
}

void AASAutoSplineBase::RedesignTheSplineElements()
{
	
}
#if WITH_EDITOR

void AASAutoSplineBase::OnSplineUpdated()
{
	RedesignTheSplineElements();	
}

#endif

void AASAutoSplineBase::CreateActorsFromSpline(bool bInSelectAfter)
{
	
}


FVector AASAutoSplineBase::GetSurfaceNormalAtLocation(const FVector& InLocation)
{
	if(!GWorld){return FVector::ZeroVector;}
	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams collisionParams(lineTraceSingleName);
	collisionParams.bTraceComplex = false;
	collisionParams.bReturnPhysicalMaterial = false;
	FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	objectParams.AddObjectTypesToQuery(ECC_Visibility);
	objectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FHitResult HitResult;
	
	GWorld->LineTraceSingleByObjectType(HitResult, InLocation, InLocation + FVector(0,0,-15000.0f), objectParams, collisionParams);
	if(HitResult.bBlockingHit)
	{
		return HitResult.Normal;
	}
	return FVector::ZeroVector;
}

void AASAutoSplineBase::SnapLocationToSurfaceIfAvailableOne(FVector& InLocation) const
{
	if(!GWorld){return;}
	FVector WorldLoc = GetActorTransform().TransformPosition(InLocation);
	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams collisionParams(lineTraceSingleName);
	collisionParams.bTraceComplex = false;
	collisionParams.bReturnPhysicalMaterial = false;
	FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	objectParams.AddObjectTypesToQuery(ECC_Visibility);
	objectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	FHitResult HitResult;
	GWorld->LineTraceSingleByObjectType(HitResult, WorldLoc, WorldLoc + FVector(0,0,-15000.0f), objectParams, collisionParams);
	if(HitResult.bBlockingHit)
	{
		InLocation = GetActorTransform().InverseTransformPosition(HitResult.Location);
	}
}


#if WITH_EDITOR

void AASAutoSplineBase::CopyTargetSpline(USplineComponent* InSplineComponent) const
{
	InSplineComponent->ClearSplinePoints();
	
	for (int32 i = 0; i < GetSplineComp()->GetNumberOfSplinePoints(); i++)
	{
		InSplineComponent->AddSplinePoint(GetSplineComp()->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local),ESplineCoordinateSpace::Local);
	}
}
#endif
