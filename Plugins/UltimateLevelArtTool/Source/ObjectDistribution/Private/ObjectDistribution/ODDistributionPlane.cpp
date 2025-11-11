// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODDistributionPlane.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"
#include "Math/Vector.h"

UODDistributionPlane::UODDistributionPlane(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Plane;
}

void UODDistributionPlane::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& PlaneData = GetToolSubsystem()->GetODToolSettings()->PlaneDistributionData;
		
		Offset = PlaneData.Offset;
		bRandomOffset = PlaneData.bRandomOffset;
		RandomOffset = PlaneData.RandomOffset;
	}
}

void UODDistributionPlane::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& PlaneData = GetToolSubsystem()->GetODToolSettings()->PlaneDistributionData;
		
		PlaneData.Offset = Offset;
		PlaneData.bRandomOffset = bRandomOffset;
		PlaneData.RandomOffset = RandomOffset;

		
		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}


FVector UODDistributionPlane::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	// Calculate rows and columns based on the total number of points
	const int32 NumRows = FMath::CeilToInt(sqrt(InLength));
	const int32 NumCols = FMath::CeilToInt(static_cast<float>(InLength) / NumRows);

	// Calculate row and column indices for the current point
	const int32 RowIndex = InIndex / NumCols;
	const int32 ColIndex = InIndex % NumCols;

	// Calculate the position of the current point
	FVector PointPosition = FVector(
		ColIndex * Offset - (NumCols - 1) * Offset * 0.5f,
		RowIndex * Offset - (NumRows - 1) * Offset * 0.5f,
		0.0f);

	if(bRandomOffset)
	{
		//Add Random Offset
		PointPosition +=
		FVector(
		FMath::FRandRange(-RandomOffset.X, RandomOffset.X),
		FMath::FRandRange(-RandomOffset.Y, RandomOffset.Y),
		FMath::FRandRange(-RandomOffset.Z, RandomOffset.Z)
		);
	}
	return PointPosition;
}


