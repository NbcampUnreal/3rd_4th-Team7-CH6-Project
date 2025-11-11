// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MAMaterialParamObject.generated.h"

class UMaterialInterface;

DECLARE_DELEGATE(FOnMaterialParamChanged);


UCLASS()

class MATERIALASSIGNMENT_API UMAMaterialParamObject : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Material Assignment", meta=(NoResetToDefault))
	TObjectPtr<UMaterialInterface> NewMaterial;

public:
	void SetMaterialParam(UMaterialInterface* InMaterialInterface){NewMaterial = InMaterialInterface;}
	UMaterialInterface* GetMaterialParam() const {return NewMaterial;}
	
	virtual void BeginDestroy() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	FOnMaterialParamChanged OnMaterialParamChanged;
};
