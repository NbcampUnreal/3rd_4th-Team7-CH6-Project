// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Building/MBTDDistribution.h"

UMBTDDistribution::UMBTDDistribution(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DistributionType = EMBDistributionType::TDGrid;
}

FVector UMBTDDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	if(GridSize.X <= 0 || GridSize.Y <= 0){return FVector::ZeroVector;}
	
	const int32 ZValue = InIndex / (GridSize.X * GridSize.Y);
	const int32 XYMultiplier = InIndex - (ZValue * GridSize.X * GridSize.Y);

	const float XValue = static_cast<float>(XYMultiplier % GridSize.X);
	const float YValue = static_cast<float>(XYMultiplier / GridSize.Y);

	return FVector(XValue,YValue,ZValue) * GridLength;
}
