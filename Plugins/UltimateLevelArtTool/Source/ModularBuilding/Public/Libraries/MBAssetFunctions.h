// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "MBAssetFunctions.generated.h"

class UTexture2D;
struct FModularBuildingAssetData;
enum class EImageFormat : int8;
enum class EImageFormat : int8;

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBAssetThumbnailFormant : uint8
{
	JPEG			UMETA(DisplayName = "JPEG"),
	PNG				UMETA(DisplayName = "PNG")
};



UCLASS(BlueprintType)
class MODULARBUILDING_API UMBAssetFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintPure, Category = "Editor|Asset")
	static FAssetData GetAssetDataFromObject(const UObject* Object);
	
	UFUNCTION(BlueprintPure, Category = "ModularBuilding|Asset")
	static FString GetAssetPathFromObject(const UObject* Object);

	UFUNCTION(BlueprintPure, Category = "ModularBuilding|Asset")
	static FAssetData GetAssetDataFromPath(const FString& AssetPath);

	UFUNCTION(BlueprintPure, Category = "ModularBuilding|Asset")
	static UTexture2D* GenerateThumbnailForAsset(const FAssetData& InAssetPath,const EMBAssetThumbnailFormant InImageFormat);

};
