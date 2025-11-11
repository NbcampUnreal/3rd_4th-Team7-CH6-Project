// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Building/MBPropDistributionBase.h"
#include "MBTDDistribution.generated.h"

/**
 * 
 */
UCLASS()
class MODULARBUILDING_API UMBTDDistribution : public UMBPropDistributionBase
{
	GENERATED_BODY()

public:
	UMBTDDistribution(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere,Category = "Distribution|3D Grid")
	FIntPoint GridSize = FIntPoint(5,5);

	UPROPERTY(EditAnywhere,Category = "Distribution|3D Grid")
	float GridLength = 500.0f;
	
	virtual EMBDistributionType GetDistributionType() override {return EMBDistributionType::TDGrid;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
};
