// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Development/ODSpawnCenter.h"
#include "Editor.h"
#include "ODBoundsVisualizerComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AODSpawnCenter::AODSpawnCenter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BilboarComponent"));
	RootComponent = BillboardComponent;
	
	BoundsVisualizerComponent = CreateDefaultSubobject<UODBoundsVisualizerComponent>(TEXT("BoundsVisualizerComponent"));

}

void AODSpawnCenter::RegenerateBoundsDrawData() const
{
	if(BoundsVisualizerComponent)
	{
		BoundsVisualizerComponent->ReDrawBounds();
	}
}

void AODSpawnCenter::UpdateDrawBoundsState() const
{
	if(BoundsVisualizerComponent)
	{
		BoundsVisualizerComponent->ReDrawBounds();
	}
}

