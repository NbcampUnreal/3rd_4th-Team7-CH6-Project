// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODDistributionCube.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"
#include "Math/Vector.h"

UODDistributionCube::UODDistributionCube(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Cube;
}

void UODDistributionCube::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& BoxData = GetToolSubsystem()->GetODToolSettings()->CubeDistributionData;
		
		ScaleDistance = BoxData.ScaleDistance;
		bRandomOffset = BoxData.bRandomOffset;
		RandomOffset = BoxData.RandomOffset;
	}
}

void UODDistributionCube::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& BoxData = GetToolSubsystem()->GetODToolSettings()->CubeDistributionData;
		
		BoxData.ScaleDistance = ScaleDistance;
		BoxData.bRandomOffset = bRandomOffset;
		BoxData.RandomOffset = RandomOffset;

		
		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}


FVector UODDistributionCube::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	FVector TargetLocation;
    
	// Calculate the position in each dimension
	const int32 SideLength = FMath::RoundToInt(FMath::Pow(InLength, 1.0f / 3.0f));
	
	const int32 Row = InIndex % SideLength;
	const int32 Depth = (InIndex % (SideLength * SideLength)) / SideLength;
	const int32 Column = InIndex / (SideLength * SideLength);
        
	// Calculate the position of the point
	TargetLocation.X = Row * ScaleDistance;
	TargetLocation.Y = Depth * ScaleDistance;
	TargetLocation.Z = Column * ScaleDistance;
	
	if(bRandomOffset)
	{
		//Add Random Offset
		return TargetLocation +=
		FVector(
		FMath::FRandRange(-RandomOffset.X, RandomOffset.X),
		FMath::FRandRange(-RandomOffset.Y, RandomOffset.Y),
		FMath::FRandRange(-RandomOffset.Z, RandomOffset.Z)
		);
	}
	return TargetLocation;
}


