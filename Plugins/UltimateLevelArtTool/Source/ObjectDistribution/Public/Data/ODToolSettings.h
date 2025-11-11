// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionData.h"
#include "ODToolSettings.generated.h"

DECLARE_DELEGATE(FOnToolSettingsSetToReset);

UCLASS(Config = ODToolSettings)
class OBJECTDISTRIBUTION_API UODToolSettings : public UObject
{
	GENERATED_BODY()
public:
	
	UPROPERTY(Config)
	FODDiskDistributionData DiskDistributionData = FODDiskDistributionData();

	UPROPERTY(Config)
	FODCubeDistributionData CubeDistributionData = FODCubeDistributionData();

	UPROPERTY(Config)
	FODLineDistributionData LineDistributionData = FODLineDistributionData();

	UPROPERTY(Config)
	FODRingDistributionData RingDistributionData = FODRingDistributionData();

	UPROPERTY(Config)
	FODSphereDistributionData SphereDistributionData = FODSphereDistributionData();

	UPROPERTY(Config)
	FODGridDistributionData GridDistributionData = FODGridDistributionData();

	UPROPERTY(Config)
	FODPlaneDistributionData PlaneDistributionData = FODPlaneDistributionData();
	
	UPROPERTY(Config)
	FODSpiralDistributionData SpiralDistributionData = FODSpiralDistributionData();

	UPROPERTY(Config)
	bool bSimulatePhysics = true;
	
	UPROPERTY(Config)
	bool bDisableSimAfterFinish = true;
	
	UPROPERTY(Config)
	float KillZ = -500.0f;
	
	UPROPERTY(Config)
	bool bDrawSpawnBounds = true;

	UPROPERTY(Config)
	FLinearColor BoundsColor = FLinearColor( 0.339158f, 0.088386f, 0.953125f);

	UPROPERTY(Config)
	FString FinishingType = FString(TEXT("Keep"));

	UPROPERTY(Config)
	FString TargetType = FString(TEXT("SM Component"));
	
	UPROPERTY(Config)
	bool bTestForCollider = false;
	
	UPROPERTY(Config)
	int32 MaxCollisionTest = 100;

	UPROPERTY(Config)
	EObjectDistributionType LastSelectedDistributionType = EObjectDistributionType::Cube;
	
	void LoadDefaultSettings();

	FOnToolSettingsSetToReset OnToolSettingsSetToReset;
};

inline void UODToolSettings::LoadDefaultSettings()
{
	DiskDistributionData = FODDiskDistributionData();

	CubeDistributionData = FODCubeDistributionData();

	LineDistributionData = FODLineDistributionData();

	RingDistributionData = FODRingDistributionData();

	SphereDistributionData = FODSphereDistributionData();

	GridDistributionData = FODGridDistributionData();

	SpiralDistributionData = FODSpiralDistributionData();

	bSimulatePhysics = true;
	
	bDisableSimAfterFinish = true;
	
	KillZ = -500.0f;
	
	bDrawSpawnBounds = true;

	BoundsColor = FLinearColor( 0.339158f, 0.088386f, 0.953125f);

	bTestForCollider = false;

	MaxCollisionTest = 100;

	FinishingType = FString(TEXT("Keep"));

	TargetType = FString(TEXT("SM Component"));

	LastSelectedDistributionType = EObjectDistributionType::Cube;
	
	SaveConfig();

	OnToolSettingsSetToReset.ExecuteIfBound();
}
