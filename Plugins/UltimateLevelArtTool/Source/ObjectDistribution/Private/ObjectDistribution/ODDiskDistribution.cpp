// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ObjectDistribution/ODDiskDistribution.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

UODDiskDistribution::UODDiskDistribution(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Disk;
}

void UODDiskDistribution::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& DiskData = GetToolSubsystem()->GetODToolSettings()->DiskDistributionData;
		
		Radius = DiskData.Radius;
		bRandZRangeCond = DiskData.bRandZRangeCond;
		RandZRange = DiskData.RandZRange;
		Chaos = DiskData.Chaos; 
	}
}

void UODDiskDistribution::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& DiskData = GetToolSubsystem()->GetODToolSettings()->DiskDistributionData;
		
		DiskData.Radius =  Radius;
		DiskData.bRandZRangeCond =  bRandZRangeCond;
		DiskData.RandZRange = RandZRange;
		DiskData.Chaos = Chaos;
		
		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}



FVector UODDiskDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	FVector TargetPoint;
	
	const float Increment = PI * (3.0f - FMath::Sqrt(5.0f));
	const float Offset = 2.0f / InLength;

	TargetPoint.Z = ((InIndex * Offset) - 1) + (Offset / 2);
	const float Distance = FMath::Sqrt(1 - FMath::Pow(TargetPoint.Z, 2));

	const float Phi = ((InIndex + 1) % InLength) * Increment;

	TargetPoint.X = FMath::Cos(Phi) * Distance * Radius;
	TargetPoint.Y = FMath::Sin(Phi) * Distance * Radius;
	
	if(Chaos)
	{
		TargetPoint = TargetPoint * FMath::RandRange(0.1f,1.5f);
	}
	
	if(bRandZRangeCond)
	{
		TargetPoint.Z += FMath::FRandRange(RandZRange.X, RandZRange.Y);
	}
	
	return TargetPoint;
}
