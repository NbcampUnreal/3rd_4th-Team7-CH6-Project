// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionBase.h"
#include "ODDiskDistribution.generated.h"

UCLASS()
class OBJECTDISTRIBUTION_API UODDiskDistribution : public UODDistributionBase
{
	GENERATED_BODY()

public:
	UODDiskDistribution(const FObjectInitializer& ObjectInitializer);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void LoadDistData() override; 
	
	UPROPERTY(EditAnywhere,Category = "Distribution")
	float Radius = 3000.0f;

	UPROPERTY(EditAnywhere,Category = "Distribution",meta=(InlineEditConditionToggle),DisplayName = "Random Z Offset")
	bool bRandZRangeCond = false;
	
	UPROPERTY(EditAnywhere,Category = "Distribution",meta=(EditCondition = "bRandZRangeCond"),DisplayName = "Random Z Offset")
	FVector2D RandZRange = FVector2D(0.0f,500.0f);

	UPROPERTY(EditAnywhere,Category = "Distribution")
	bool Chaos = false;
	
	virtual EObjectDistributionType GetDistributionType() override {return EObjectDistributionType::Disk;}
private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
	
};
