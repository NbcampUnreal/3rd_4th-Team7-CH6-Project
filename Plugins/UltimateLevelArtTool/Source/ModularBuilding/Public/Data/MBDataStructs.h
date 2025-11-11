// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/MBModularEnum.h"
#include "MBDataStructs.generated.h"


USTRUCT(BlueprintType)
struct FDuplicationData
{
	GENERATED_BODY()
	
	FORCEINLINE FDuplicationData();

	explicit FORCEINLINE FDuplicationData(EMBAxis DupAxis, int32 NumOfDup, float Offset,bool bDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ModularDuplication")
	EMBAxis DupAxis = EMBAxis::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ModularDuplication")
	int32 NumOfDup = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ModularDuplication")
	float Offset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ModularDuplication")
	bool bDirection = true;
};


FDuplicationData::FDuplicationData()
{
}

inline FDuplicationData::FDuplicationData(EMBAxis InDupAxis, int32 InNumOfDup, float InOffset, bool InbDirection) : DupAxis(InDupAxis), NumOfDup(InNumOfDup), Offset(InOffset),bDirection(InbDirection)
{
	
}

USTRUCT(BlueprintType)
struct FDuplicationFilters
{
	GENERATED_BODY()
	
	FORCEINLINE FDuplicationFilters();

	explicit FORCEINLINE FDuplicationFilters(bool Hole,bool InRectangle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = ModularDuplication)
	bool Hole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = ModularDuplication)
	bool Rectangle;
	
};
FDuplicationFilters::FDuplicationFilters()
{
	Hole = false;
	Rectangle = false;
}

inline FDuplicationFilters::FDuplicationFilters(bool InHole,bool InRectangle) : Hole(InHole),Rectangle(InRectangle)
{
}