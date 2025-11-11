// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBModularBuildingSettings.generated.h"

class UBorder;
class UCheckBox;
class UEditableTextBox;
class UButton;
class UTextBlock;
class UPanelWidget;

enum class EMBAxis : uint8;


UCLASS()
class MODULARBUILDING_API UMBModularBuildingSettings : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<class UMBMBuildingSettingsObj> BuildingSettingsObj;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PlacementTypeBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlacementTypeText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UDetailsView> MBDetails;
	
	UFUNCTION()
	void PlacementTypeBtnPressed();
private:
	UFUNCTION()
	void OnPlacementTypeChanged();

	void RefreshPlacementText() const;
};

