// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ch6GameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class Ach6GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	Ach6GameMode();
};



