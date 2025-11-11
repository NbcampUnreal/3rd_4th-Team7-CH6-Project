// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FAutoSplineEditorModule : public IModuleInterface
{
public:

	/* IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITOR
	
private:
	void RegisterCustomDetailClasses() const;
	void UnregisterCustomDetailClasses() const;

#endif
	
};
