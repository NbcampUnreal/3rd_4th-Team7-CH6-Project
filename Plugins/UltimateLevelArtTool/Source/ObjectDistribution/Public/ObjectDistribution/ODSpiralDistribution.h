// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionBase.h"
#include "ODSpiralDistribution.generated.h"

UCLASS()
class OBJECTDISTRIBUTION_API UODSpiralDistribution : public UODDistributionBase
{
	GENERATED_BODY()

public:
	UODSpiralDistribution(const FObjectInitializer& ObjectInitializer);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void LoadDistData() override; 
	
	UPROPERTY(EditAnywhere,Category = "Distribution",meta=(ClampMin=1))
	int32 Rotation = 100;

	UPROPERTY(EditAnywhere,Category = "Distribution")
	float Length = 25.0f;
	
	UPROPERTY(EditAnywhere,Category = "Distribution")
	int32 ScaleDistance = 250.0f;
	
	virtual EObjectDistributionType GetDistributionType() override {return EObjectDistributionType::Spiral;}
private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
	
};
