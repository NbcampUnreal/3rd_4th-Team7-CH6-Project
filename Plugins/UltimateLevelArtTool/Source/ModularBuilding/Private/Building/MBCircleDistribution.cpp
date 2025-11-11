// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Building/MBCircleDistribution.h"

UMBCircleDistribution::UMBCircleDistribution(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EMBDistributionType::Circle;
}

FVector UMBCircleDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	const float AngleDeg = ((ArcAngle / InLength) * InIndex) + Offset;
	return FVector(Radius,0.0f,0.0f).RotateAngleAxis(AngleDeg, FVector::UpVector);
}
