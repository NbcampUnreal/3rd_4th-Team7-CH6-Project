// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MBMainScreenInterface.generated.h"

struct FDuplicationData;
struct FDuplicationFilters;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMBMainScreenInterface : public UInterface
{
	GENERATED_BODY()
};

class MODULARBUILDING_API IMBMainScreenInterface
{
	GENERATED_BODY()

public:
	FORCEINLINE virtual UObject* GetBuildingManager() = 0;
	FORCEINLINE virtual void ChangeTheCategory() = 0;
	FORCEINLINE virtual void CreateAssetClicked(const FString AssetName) = 0;
	FORCEINLINE virtual void StartModularDuplicationFromSettings(const FDuplicationData& InDuplicationData) = 0;
	FORCEINLINE virtual void AddNewCollection() = 0;
	FORCEINLINE virtual void TurnOffCollectionState(const int32 InIndex) = 0;
	FORCEINLINE virtual TArray<FDuplicationData*> GetExistingDuplicationData() = 0;
	FORCEINLINE virtual void ApplyModularDuplicationPressed() = 0;
	FORCEINLINE virtual void ApplyModularDuplicationFilter(const FDuplicationFilters& InDuplicationFilters) = 0;
	FORCEINLINE virtual FDuplicationFilters* GetExistingDuplicationFilter() = 0;
	FORCEINLINE virtual void UpdateTheCollectionTabState() = 0;
	FORCEINLINE virtual void AssetTypeRemoved() = 0;
	FORCEINLINE virtual void CollectionRemoved() = 0;
	FORCEINLINE virtual void CollectionRestored() = 0;
	FORCEINLINE virtual void CollectionNameChanged(const FName& InOldName,const FName& InNewName) = 0;
	FORCEINLINE virtual void UpdateCategoryWindow() = 0;
	FORCEINLINE virtual void SetMultiplePlacementAmountText(const FString& InText) const = 0;
	FORCEINLINE virtual void ResetToolWindowInterface() = 0;
};
