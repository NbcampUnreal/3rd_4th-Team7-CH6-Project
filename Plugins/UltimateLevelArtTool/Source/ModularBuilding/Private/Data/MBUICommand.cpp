// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Data/MBUICommand.h"
#include "Framework/Commands/Commands.h"

#define LOCTEXT_NAMESPACE "FUltimateLevelArtToolModule"

void FMBUICommands::RegisterCommands()
{
	UI_COMMAND(
	PlaceModularActor,
	"Place Selected Modular Actor",
	"Place moved actor in level, once triggered, actor will be placed",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::LeftMouseButton)
	);	

	UI_COMMAND(
	RotateToLeft,
	"Rotate To Left",
	"Rotate Selected Actor CounterClockwise",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::Z)
	);

	UI_COMMAND(
	RotateToRight,
	"Rotate To Right",
	"Rotate Selected Actor Clockwise",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::C)
	);

	UI_COMMAND(
	DeleteAsset,
	"Delete Asset",
	"Delete Clicked Asset",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::RightMouseButton,EModifierKey::Shift)
	);
	
}

#undef LOCTEXT_NAMESPACE
