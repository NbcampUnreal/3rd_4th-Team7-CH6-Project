// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ODPaletteItemToolTip.generated.h"

class URichTextBlock;
class UTextBlock;
/**
 * 
 */
UCLASS()
class OBJECTDISTRIBUTION_API UODPaletteItemToolTip : public UUserWidget
{
	GENERATED_BODY()


public:
	void SetTooltipData(const FName& InPresetName, const FAssetData& InAssetData) const; 

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AssetNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> AssetInfoText;
};
