// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionBase.h"
#include "ODTDDistribution.generated.h"


UCLASS()
class OBJECTDISTRIBUTION_API UODTDDistribution : public UODDistributionBase
{
	GENERATED_BODY()

public:
	UODTDDistribution(const FObjectInitializer& ObjectInitializer);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void LoadDistData() override; 
	
	UPROPERTY(EditAnywhere,Category = "Distribution")
	FIntPoint GridSize = FIntPoint(5,5);

	UPROPERTY(EditAnywhere,Category = "Distribution")
	float GridLength = 500.0f;

	UPROPERTY(EditAnywhere,Category = "Distribution")
	bool Chaos = false;
	
	virtual EObjectDistributionType GetDistributionType() override {return EObjectDistributionType::Grid;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
};
