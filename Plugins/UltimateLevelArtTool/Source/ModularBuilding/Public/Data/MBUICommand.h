// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

//#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FMBUICommands : public TCommands<FMBUICommands>
{
public:
	FMBUICommands() : TCommands<FMBUICommands>(
	TEXT("Modular Building"),
	FText::FromString(TEXT("Modular Building Commands")),
	NAME_None,
	TEXT("Modular Building")
	) {}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> PlaceModularActor;
	TSharedPtr<FUICommandInfo> RotateToLeft;
	TSharedPtr<FUICommandInfo> RotateToRight;
	TSharedPtr<FUICommandInfo> DeleteAsset;
};

