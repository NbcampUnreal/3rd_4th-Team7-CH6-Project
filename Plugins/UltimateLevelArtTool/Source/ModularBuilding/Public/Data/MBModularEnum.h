// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBModularEnum.generated.h"

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EBuildingCategory : uint8
{
	None			UMETA(DisplayName = "None"),
	Modular			UMETA(DisplayName = "Modular"),
	Prop			UMETA(DisplayName = "Prop")
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EPlacementMode: uint8
{
	None			UMETA(DisplayName = "None"),
	SingleModular	UMETA(DisplayName = "SingleModular"),
	MultiModular	UMETA(DisplayName = "MultiModular"),
	SingleProp		UMETA(DisplayName = "Left"),
	MultiProp		UMETA(DisplayName = "Right")
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EPlacementType: uint8
{
	Single		UMETA(DisplayName = "Single"),
	Multiple	UMETA(DisplayName = "Multiple"),
};

inline EPlacementType NextPlacementType(const EPlacementType& CurrentType)
{
	switch (CurrentType) {default: return {};
	case EPlacementType::Single: return EPlacementType::Multiple;
	case EPlacementType::Multiple: return EPlacementType::Single;
	}
}


UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBWorkingMode: uint8
{
	None			UMETA(DisplayName = "None"),
	Placement		UMETA(DisplayName = "Placement"),
	ModActorAction	UMETA(DisplayName = "ModActorAction"),
	PropActorAction	UMETA(DisplayName = "PropActorAction")
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EPlacementStatus : uint8
{
	None			UMETA(DisplayName = "None"),
	NotPlaceable	UMETA(DisplayName = "NotPlaceable"),
	Placeable		UMETA(DisplayName = "Placeable"),
	Replaceable		UMETA(DisplayName = "Replaceable")
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EModularDirection : uint8
{
	None			UMETA(DisplayName = "None"),
	Front			UMETA(DisplayName = "Front"),
	Back			UMETA(DisplayName = "Back"),
	Left			UMETA(DisplayName = "Left"),
	Right			UMETA(DisplayName = "Right")
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class ESettingsMenuType : uint8
{
	None			UMETA(DisplayName = "None"),
	ModBuilding		UMETA(DisplayName = "Front"),
	PropBuilding	UMETA(DisplayName = "Back"),
	ModActorAction	UMETA(DisplayName = "Left"),
	ModPropAction	UMETA(DisplayName = "Right")
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBAxis : uint8
{
	None			UMETA(DisplayName = "None"),
	AxisX			UMETA(DisplayName = "X"),
	AxisY			UMETA(DisplayName = "Y"),
	AxisZ			UMETA(DisplayName = "Z"),
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBDistributionType : uint8
{
	Box				UMETA(DisplayName = "Box"),
	Circle			UMETA(DisplayName = "Circle"),
	Sphere			UMETA(DisplayName = "Sphere"),
	TDGrid			UMETA(DisplayName = "3DGrid")
};


UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBPropOrientation : uint8
{
	Keep			UMETA(DisplayName = "Keep"),
	Inside			UMETA(DisplayName = "Inside"),
	Outside			UMETA(DisplayName = "Outside"),
	RandomZ			UMETA(DisplayName = "Random Around"),
	Random			UMETA(DisplayName = "Random"),
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBMeshConversionType : uint8
{
	Sm			UMETA(DisplayName =  "SM Component"),
	Ism     	UMETA(DisplayName = "ISM Component"),
	HIsm		UMETA(DisplayName = "HISM Component"),
};

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class EMBModularSnapType : uint8
{
	SMBounds			UMETA(DisplayName =  "Static Mesh Bounds"),
	BoundCorrection     UMETA(DisplayName = "Bound Correction Sensitivity"),
	SMCollision			UMETA(DisplayName = "Static Mesh Collision"),
};


USTRUCT()
struct FDistributionBaseData {
	GENERATED_BODY()

	FDistributionBaseData();
	
	UPROPERTY()
	bool bActorSelected = false;
	UPROPERTY()
	bool bOneActorDistribution = false;
	UPROPERTY()
	EMBDistributionType DistributionType = EMBDistributionType::Box;
	UPROPERTY()
	TObjectPtr<AActor> ActorToFollow = nullptr;
	UPROPERTY()
	FVector Location = FVector::ZeroVector;
	UPROPERTY()
	EMBPropOrientation Orientation = EMBPropOrientation::Keep;
};

FORCEINLINE FDistributionBaseData::FDistributionBaseData()
{
	
}
