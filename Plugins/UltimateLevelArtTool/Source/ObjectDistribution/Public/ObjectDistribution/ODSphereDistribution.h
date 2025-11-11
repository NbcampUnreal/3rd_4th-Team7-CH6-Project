// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionBase.h"
#include "ODSphereDistribution.generated.h"

UCLASS()
class OBJECTDISTRIBUTION_API UODSphereDistribution : public UODDistributionBase
{
	GENERATED_BODY()

public:
	UODSphereDistribution(const FObjectInitializer& ObjectInitializer);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void LoadDistData() override; 
	
	UPROPERTY(EditAnywhere,Category = "Distribution")
	float SpiralWinding = 0.5f;

	UPROPERTY(EditAnywhere,Category = "Distribution")
	float SphereRadius = 1000.0f;

	UPROPERTY(EditAnywhere,Category = "Distribution")
	bool Chaos = false;
	
	virtual EObjectDistributionType GetDistributionType() override {return EObjectDistributionType::Sphere;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
};
