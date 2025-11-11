// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBAssetSlot.generated.h"

class UTextBlock;
class UButton;
class UImage;

UCLASS()
class MODULARBUILDING_API UMBAssetSlot : public UUserWidget
{
	GENERATED_BODY()

	uint8  bSelectionState:1;

public:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AssetSelectionBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MeshNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MeshImage;

	UPROPERTY()
	FSoftObjectPath AssetPath;

	UFUNCTION()
	void AssetSelectionBtnPressed();
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	void SetSlotParams(const UStaticMesh* StaticMeshAsset);

	UFUNCTION()
	void SetSelectionState(const bool InSelectionState);
};
