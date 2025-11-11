// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ODSpawnCenter.generated.h"

class UODBoundsVisualizerComponent;
class UBillboardComponent;

UCLASS()
class OBJECTDISTRIBUTION_API AODSpawnCenter : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UODBoundsVisualizerComponent> BoundsVisualizerComponent;
	
public:	
	AODSpawnCenter();

	TObjectPtr<UBillboardComponent> GetBillboardComp() const {return BillboardComponent;}
	
	TObjectPtr<UODBoundsVisualizerComponent> GetBoundsVisualizerComp() const {return BoundsVisualizerComponent;}

#pragma region DistributionVisualization
public:
		void RegenerateBoundsDrawData() const;
		void UpdateDrawBoundsState() const;

	


	

#pragma endregion  DistributionVisualization
	
};
