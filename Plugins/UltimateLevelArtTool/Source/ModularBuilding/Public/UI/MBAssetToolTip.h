// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBAssetToolTip.generated.h"

class URichTextBlock;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MODULARBUILDING_API UMBAssetToolTip : public UUserWidget
{
	GENERATED_BODY()


public:
	void SetTooltipData(const FAssetData& InAssetData) const; 

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AssetNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> AssetInfoText;
};
