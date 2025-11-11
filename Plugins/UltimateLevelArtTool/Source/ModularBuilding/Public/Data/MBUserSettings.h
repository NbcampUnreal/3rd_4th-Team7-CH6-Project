// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MBUserSettings.generated.h"

UCLASS(BlueprintType,Config = MBUserSettings)
class MODULARBUILDING_API UMBUserSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Placement",meta = (ClampMin = 5000.0f,ClampMax = 100000.0f))
	float ObjectTracingDistance = 50000.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Placement",meta = (ClampMin = 500.0f,ClampMax = 50000.0f))
	float FreePlacementDistance = 3500.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging")
	bool UsePreviewShader = false;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging")
	FLinearColor PlaceableColor = FLinearColor::FromSRGBColor(FColor::FromHex("94FFB0FF"));
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging")
	FLinearColor NotPlaceableColor = FLinearColor::Red;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging")
	FLinearColor ReplaceableColor = FLinearColor::FromSRGBColor(FColor::FromHex("3EAFD5FF"));

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging")
	bool EnableDirectionDebugger = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging")
	FLinearColor DirectionDebugColor = FLinearColor::FromSRGBColor(FColor::FromHex("3EAFD5FF"));

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debugging",meta = (ClampMin = 0.1,ClampMax = 50.0))
	float DebugThickness = 5.0f;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "User Interface")
	FLinearColor AssetSelectionColor = FLinearColor::FromSRGBColor(FColor::FromHex("228C38FF"));

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "User Interface")
	FLinearColor CollectionSelectionColor = FLinearColor::FromSRGBColor(FColor::FromHex("176B29FF"));

	void ResetSettingParams();
};

inline void UMBUserSettings::ResetSettingParams()
{
	ObjectTracingDistance = 50000.0f;
	FreePlacementDistance = 3500.0f;
	UsePreviewShader = false;
	PlaceableColor = FLinearColor::FromSRGBColor(FColor::FromHex("94FFB0FF"));
	NotPlaceableColor = FLinearColor::Red;
	ReplaceableColor = FLinearColor::FromSRGBColor(FColor::FromHex("3EAFD5FF"));
	EnableDirectionDebugger = false;
	DirectionDebugColor = FLinearColor::FromSRGBColor(FColor::FromHex("3EAFD5FF"));
	DebugThickness = 5.0f;
	AssetSelectionColor = FLinearColor::FromSRGBColor(FColor::FromHex("228C38FF"));
	CollectionSelectionColor = FLinearColor::FromSRGBColor(FColor::FromHex("176B29FF"));
}
