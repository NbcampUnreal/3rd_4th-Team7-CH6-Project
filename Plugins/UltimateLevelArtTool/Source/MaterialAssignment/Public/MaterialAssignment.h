// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


class FMaterialAssignmentModule : public IModuleInterface
{
public:

	/* IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
