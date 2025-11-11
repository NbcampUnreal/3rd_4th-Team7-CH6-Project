// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Building/MBSphereDistribution.h"

UMBSphereDistribution::UMBSphereDistribution(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EMBDistributionType::Sphere;
}

FVector UMBSphereDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	//Y Value
	float YValue = static_cast<float>(InIndex) / (static_cast<float>(InLength) - 1.0f);
	YValue *= 2.0f;
	YValue = 1.0f - YValue;
	
	const float Val1 = FMath::Sqrt(1.0f - FMath::Square(YValue));
	float Val2 = DOUBLE_PI * 3 - FMath::Sqrt(SpiralWinding);
	Val2 *= static_cast<float>(InIndex);

	FVector Loc = FVector(Val1 * FMath::Cos(Val2),YValue,Val1 * FMath::Sin(Val2));
	Loc *= SphereRadius;
	return Loc;
}
