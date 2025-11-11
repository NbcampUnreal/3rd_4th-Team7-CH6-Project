// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MAToolWindowInterface.generated.h"

class UMaterialInterface;

UINTERFACE(MinimalAPI)
class UMAToolWindowInterface : public UInterface
{
	GENERATED_BODY()
};

class MATERIALASSIGNMENT_API IMAToolWindowInterface
{
	GENERATED_BODY()

public:
	virtual void MaterialPropertyChanged(bool bIncreaseCounter) = 0;
	virtual void ApplySingleMaterialChange(const FName& SlotName , UMaterialInterface* InMaterialInterface) = 0;
	virtual bool RenameMaterialSlot(const FName& CurrentSlotName,const FName& NewSlotName) = 0;
	virtual void DeleteSlot(const FName& InSlotName) = 0;

	
};
