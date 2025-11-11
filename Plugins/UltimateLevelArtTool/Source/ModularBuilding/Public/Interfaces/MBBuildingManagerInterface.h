// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MBBuildingManagerInterface.generated.h"

class UMaterialInterface;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMBBuildingManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MODULARBUILDING_API IMBBuildingManagerInterface
{
	GENERATED_BODY()
	
public:
	virtual void ApplyMaterialToDuplicatedActors(const int32& InSlotIndex, UMaterialInterface* InMaterialInterface) = 0;
	virtual void ResetModularDuplication() = 0;
	virtual void ApplyScaleRate() = 0;
	virtual void ResetPropSnapRotation() = 0;
	
};
