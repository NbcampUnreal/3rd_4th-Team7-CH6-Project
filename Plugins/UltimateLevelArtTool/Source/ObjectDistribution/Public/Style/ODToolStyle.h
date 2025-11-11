// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

class FODToolStyle
{
public:
	static void InitializeToolStyle();
	static void ShutDownStyle();

private:
	static FName ToolStyleName;

	static TSharedRef<FSlateStyleSet> CreateToolSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> CreatedToolSlateStyleSet;

public:
	static const ISlateStyle& Get()
	{
		return *FSlateStyleRegistry::FindSlateStyle(FName("ODToolStyle"));
	}
	
	static FName GetToolStyleName(){return ToolStyleName;}

	static TSharedRef<FSlateStyleSet> GetCreatedToolSlateStyleSet() {return CreatedToolSlateStyleSet.ToSharedRef(); }
};
