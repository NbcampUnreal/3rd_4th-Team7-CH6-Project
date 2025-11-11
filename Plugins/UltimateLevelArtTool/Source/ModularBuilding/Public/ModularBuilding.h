// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#define MODULAR_ACTOR_TAG TEXT("ModularActor")
#define MODULAR_TAG TEXT("Modular")
#define PROP_TAG TEXT("Prop")

class FModularBuildingModule : public IModuleInterface
{
public:

	/* IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
};
