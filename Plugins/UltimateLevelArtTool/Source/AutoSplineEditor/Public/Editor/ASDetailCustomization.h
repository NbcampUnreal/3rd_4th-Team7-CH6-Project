// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FASDeformMeshDetailCustomization : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static TSharedRef< IDetailCustomization > MakeInstance();
};

class FASCustomClassDetailCustomization : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static TSharedRef< IDetailCustomization > MakeInstance();
};

class FASMeshDistributionDetailCustomization : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static TSharedRef< IDetailCustomization > MakeInstance();
};

#endif