// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBModularEnum.h"
#include "Engine/DataTable.h"
#include "MBModularAssetData.generated.h"

class UStaticMesh;

USTRUCT(BlueprintType)
struct FModularBuildingAssetData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	FModularBuildingAssetData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolAsset")
	FName MeshType = FName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolAsset")
	EBuildingCategory MeshCategory = EBuildingCategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolAsset")
	TSoftObjectPtr<UStaticMesh> AssetReference = nullptr;
};

FORCEINLINE FModularBuildingAssetData::FModularBuildingAssetData()
{
	
}
