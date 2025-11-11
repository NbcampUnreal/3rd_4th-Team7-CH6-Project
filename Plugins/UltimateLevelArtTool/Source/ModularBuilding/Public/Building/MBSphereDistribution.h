// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Building/MBPropDistributionBase.h"
#include "MBSphereDistribution.generated.h"

UCLASS()
class MODULARBUILDING_API UMBSphereDistribution : public UMBPropDistributionBase
{
	GENERATED_BODY()

public:
	UMBSphereDistribution(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere,Category = "Distribution|Sphere")
	float SpiralWinding = 0.5f;

	UPROPERTY(EditAnywhere,Category = "Distribution|Sphere")
	float SphereRadius = 500.0f;
	
	virtual EMBDistributionType GetDistributionType() override {return EMBDistributionType::Sphere;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
};
