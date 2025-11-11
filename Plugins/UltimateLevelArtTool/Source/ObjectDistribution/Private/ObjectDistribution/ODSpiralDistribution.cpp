// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODSpiralDistribution.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

UODSpiralDistribution::UODSpiralDistribution(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Spiral;
}

void UODSpiralDistribution::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& HelixData = GetToolSubsystem()->GetODToolSettings()->SpiralDistributionData;

		Rotation = HelixData.Rotation;
		Length = HelixData.Length;
		ScaleDistance = HelixData.ScaleDistance;
	}
}

void UODSpiralDistribution::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& HelixData = GetToolSubsystem()->GetODToolSettings()->SpiralDistributionData;

		HelixData.Rotation = Rotation;
		HelixData.Length = Length;
		HelixData.ScaleDistance = ScaleDistance;

		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}

FVector UODSpiralDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	FVector ResultLocation;
    
	const float NormalizedIndex = static_cast<float>(InIndex) / static_cast<float>(InLength - 1);
	const float Angle = NormalizedIndex * Rotation * 2.0f * PI; 
	const float Z = NormalizedIndex * Length; 
        
	const float X = FMath::Cos(Angle);
	const float Y = FMath::Sin(Angle);
        
	ResultLocation.X = X * ScaleDistance; 
	ResultLocation.Y = Y * ScaleDistance;
	ResultLocation.Z = Z * ScaleDistance;
    
	return ResultLocation;
}
