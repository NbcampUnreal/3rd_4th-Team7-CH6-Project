// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODRingDistribution.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

UODRingDistribution::UODRingDistribution(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Ring;
}

void UODRingDistribution::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& RingData = GetToolSubsystem()->GetODToolSettings()->RingDistributionData;
		ArcAngle = RingData.ArcAngle;
		Offset =  RingData.Offset;
		Radius = RingData.Radius;
		bRandZRangeCond = RingData.bRandZRangeCond;
		RandZRange = RingData.RandZRange;
		Chaos = RingData.Chaos;
	}
}

void UODRingDistribution::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& RingData = GetToolSubsystem()->GetODToolSettings()->RingDistributionData;
		RingData.ArcAngle = ArcAngle;
		RingData.Offset = Offset;
		RingData.Radius = Radius;
		RingData.bRandZRangeCond = bRandZRangeCond;
		RingData.RandZRange = RandZRange;
		RingData.Chaos = Chaos;
				
		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}

FVector UODRingDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	const float AngleDeg = ((ArcAngle / InLength) * InIndex) + Offset;
	FVector FinalLoc =  FVector(Radius,0.0f,0.0f).RotateAngleAxis(AngleDeg, FVector::UpVector);
	
	if(Chaos)
	{
		FinalLoc = FinalLoc * FMath::RandRange(0.2f,1.0f);
	}

	if(bRandZRangeCond)
	{
		FinalLoc.Z += FMath::FRandRange(RandZRange.X, RandZRange.Y);
	}
	
	return FinalLoc;
}
