// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBMActionSettingsObj.generated.h"

enum class EMBAxis : uint8;

UCLASS()
class MODULARBUILDING_API UMBMActionSettingsObj : public UObject
{
	GENERATED_BODY()

public:
	UMBMActionSettingsObj(const FObjectInitializer& ObjectInitializer);
	
protected:

	UPROPERTY(EditAnywhere, Category = "Multiple Modular Duplication",meta=(ClampMin = 1,NoResetToDefault))
	int32 NumberOfDuplications;

	UPROPERTY(EditAnywhere, Category = "Multiple Modular Duplication",meta=(NoResetToDefault,ValidEnumValues = "AxisX,AxisY,AxisZ"))
	EMBAxis DuplicationAxis;

	UPROPERTY(EditAnywhere, Category = "Multiple Modular Duplication",meta=(NoResetToDefault))
	float Offset;

	UPROPERTY(EditAnywhere, Category = "Multiple Modular Duplication",meta=(NoResetToDefault),DisplayName="Positive Direction")
	bool bPositiveDirection;

	UFUNCTION(CallInEditor, Category = "Multiple Modular Duplication")
	void Duplicate();
	float GetMMHalfLengthOfAxis(const AActor* InActor, const EMBAxis& InAxis) const;
};