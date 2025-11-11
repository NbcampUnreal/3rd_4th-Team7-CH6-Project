// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODSphereDistribution.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

UODSphereDistribution::UODSphereDistribution(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Sphere;
}

void UODSphereDistribution::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& SphereData = GetToolSubsystem()->GetODToolSettings()->SphereDistributionData;
		SpiralWinding = SphereData.SpiralWinding;
		SphereRadius = SphereData.SphereRadius;
		Chaos = SphereData.Chaos;
	}
}

void UODSphereDistribution::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive){return;}

	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& SphereData = GetToolSubsystem()->GetODToolSettings()->SphereDistributionData;
		SphereData.SpiralWinding = SpiralWinding;
		SphereData.SphereRadius = SphereRadius;
		SphereData.Chaos = Chaos;

		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}

FVector UODSphereDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	//Y Value
	float YValue = static_cast<float>(InIndex) / (static_cast<float>(InLength) - 1.0f);
	YValue *= 2.0f;
	YValue = 1.0f - YValue;
	
	const float Val1 = FMath::Sqrt(1.0f - FMath::Square(YValue));
	float Val2 = DOUBLE_PI * 3 - FMath::Sqrt(SpiralWinding);
	Val2 *= static_cast<float>(InIndex);

	FVector TargetLocation = FVector(Val1 * FMath::Cos(Val2),YValue,Val1 * FMath::Sin(Val2));
	TargetLocation *= SphereRadius;

	if(Chaos)
	{
		TargetLocation *= FMath::RandRange(FMath::RandRange(1,12) < 10 ? 0.3f : 0.1f,1.0f);
	}
	return  TargetLocation;
}
