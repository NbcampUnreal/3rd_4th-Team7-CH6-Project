// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "MAFunctions.generated.h"

enum class EMBAssetThumbnailFormant : uint8;
class UTexture2D;
class UStaticMesh;


UCLASS()
class MATERIALASSIGNMENT_API UMAFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Material Assignment")
	static FAssetData FindTheMostSuitableMaterialForTheNameSlot(const FString& InSlotName, const TArray<FAssetData>& MaterialList);

	UFUNCTION(BlueprintCallable, Category = "Material Assignment")
	static void GetAllMaterialInstances(TArray<FAssetData>& OutAssets);

	UFUNCTION(BlueprintCallable, Category = "Material Assignment")
	static bool IsMaterialInUsed(UStaticMesh* InStaticMesh, int32 InMaterialIndex);
};
