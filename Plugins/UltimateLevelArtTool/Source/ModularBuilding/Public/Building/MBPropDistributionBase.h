// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/MBModularEnum.h"
#include "MBPropDistributionBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnDistributionTypeChangedSignature, EMBDistributionType);

UCLASS()
class MODULARBUILDING_API UMBPropDistributionBase : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<class UMBToolSubsystem> ToolSettingsSubsystem;

	void SetToolSettings();
	
	UPROPERTY()
	bool bActorSelected;

	UPROPERTY()
	bool bOneActorDistribution;

	UPROPERTY()
	TArray<AActor*> DistributionActors;
	
public:
	UPROPERTY(EditAnywhere,Category = "Distribution",meta=(ClampMin=1,NoSpinbox = true,NoResetToDefault))
	int32 NumberOfDistribution = 1;
	
	UPROPERTY(EditAnywhere,Category = "Distribution" , meta=(NoResetToDefault))
	EMBDistributionType DistributionType = EMBDistributionType::Box;
	
	UPROPERTY(EditAnywhere,Category = "Distribution")
	TObjectPtr<AActor> ActorToFollow;

	UPROPERTY(EditAnywhere,meta=(MakeEditWidget = true),Category = "Distribution")
	FVector Location;

	UPROPERTY(EditAnywhere,Category = "Distribution" , meta=(NoResetToDefault))
	EMBPropOrientation Orientation;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	FOnDistributionTypeChangedSignature OnDistributionTypeChangedSignature;


	void SetupDistribution();
	
	void ExecuteDistribution();

private:
	void CheckForNumberChanges();


	
protected:
	virtual EMBDistributionType GetDistributionType() {return EMBDistributionType::Box;}
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength);
	FRotator CalculateRotation(const FVector& InLocation) const;

public:
	void SetDistributionBaseSettings(const FDistributionBaseData& InDistributionBaseData);
	FDistributionBaseData GetDistributionBaseSettings() const;
};
