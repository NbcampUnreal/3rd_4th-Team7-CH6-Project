// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "ODAssetFunctions.generated.h"

class UTexture2D;
struct FModularBuildingAssetData;

UCLASS(BlueprintType)
class OBJECTDISTRIBUTION_API UODAssetFunctions : public UBlueprintFunctionLibrary
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
	static UTexture2D* GenerateThumbnailForSM(const FAssetData& InAssetPath);

};
