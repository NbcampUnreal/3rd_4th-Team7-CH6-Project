// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/MBModularEnum.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MBExtendedMath.generated.h"

/**
 * 
 */
UCLASS()
class MODULARBUILDING_API UMBExtendedMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static float GetAxisOfVector(const EMBAxis InAxis, const FVector& InVector);

	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static void SetAxisOfVector(const EMBAxis InAxis, FVector& InVector, const float& InNewValue);

	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static void GetHighestAxisAndDirectionOfVector(const FVector& InVector, EMBAxis& OutAxis,bool& OutDirection);

	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static EMBAxis GetSecAxis(const EMBAxis& InWorkingAxis, const EMBAxis& InBlockedAxis);

	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static FVector GetAxisVectorWithEnumValue(const EMBAxis& InAxis);

	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static EMBAxis GetAxisEnumOfDirectionVector(const FVector& InVector);

	UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	static void GetNearestAxisOfVector(const FVector& InVector, EMBAxis& OutAxis,FVector& OutDirection);

	// UFUNCTION(BlueprintPure, Category = "Editor/MBMath", meta = (Keywords = "Editor Math"))
	// static void GetNearestHitAxisAndDirection(const FHitResult& InHitResult, const AActor* RefActor,FVector& OutDirection,EMBAxis& OutAxis);
	
};
