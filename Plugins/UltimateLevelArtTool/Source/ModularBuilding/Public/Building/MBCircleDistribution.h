// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Building/MBPropDistributionBase.h"
#include "MBCircleDistribution.generated.h"

/**
 * 
 */
UCLASS()
class MODULARBUILDING_API UMBCircleDistribution : public UMBPropDistributionBase
{
	GENERATED_BODY()

public:
	UMBCircleDistribution(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere,Category = "Distribution|Circle")
	float ArcAngle = 360.0f;

	UPROPERTY(EditAnywhere,Category = "Distribution|Circle")
	float Offset =  0.0f;

	UPROPERTY(EditAnywhere,Category = "Distribution|Circle")
	float Radius = 500.0f;

	virtual EMBDistributionType GetDistributionType() override {return EMBDistributionType::Circle;}


private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
	
};
