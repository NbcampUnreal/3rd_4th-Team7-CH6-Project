// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Building/MBDistributionBox.h"
#include "Math/Vector.h"

UMBDistributionBox::UMBDistributionBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DistributionType = EMBDistributionType::Box;
}

FVector UMBDistributionBox::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	return FVector(
		FMath::FRandRange(-SpawnRange.X, SpawnRange.X),
		FMath::FRandRange(-SpawnRange.Y, SpawnRange.Y),
		FMath::FRandRange(-SpawnRange.Z, SpawnRange.Z)
	);
}


