// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MBActorFunctions.generated.h"

enum class EBuildingCategory : uint8;


UCLASS()
class MODULARBUILDING_API UMBActorFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static void FilterForCategory(const EBuildingCategory InCategory,TArray<AActor*>& InActors);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static TArray<AActor*> GetAllActorsUnderTheFolderPath(const FName& InFolderPath);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static FBox GetAllActorsTransportPoint(const TArray<AActor*>& InActors);
	
	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static FBox GetAllActorsTransportPointWithExtents(const TArray<AActor*>& InActors);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static void FilterModularActors(TArray<AActor*>& InActors);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static TArray<AActor*>  GetAllModularActorsInWorld();

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static bool IsActorFromTool(const AActor* InActor);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static AActor* GetActorWithObjectName(const FString& InName);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static TArray<AActor*> GetAllActorsWithGivenObjectNames(const TArray<FString>& InNames);

	UFUNCTION(Category = "Editor/MBActor", meta = (Keywords = "Modular Actor"))
	static EBuildingCategory GetBuildingCategory(const AActor* InActor);
};

