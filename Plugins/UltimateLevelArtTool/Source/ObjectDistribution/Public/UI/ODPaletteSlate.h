// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Components/PanelWidget.h"
#include "ODPaletteSlate.generated.h"

class UDetailsView;
class UODToolSubsystem;

DECLARE_DELEGATE_OneParam(FOnAssetDroppedSignature, TArrayView<FAssetData>)
DECLARE_DELEGATE(FOnPaletteBackgroundPressedSignature)

// UENUM()
enum class EPaletteButtonType : uint8
{
	Create,
	Shuffle,
	Delete,
	SelectAll,
	SelectSpawnCenter,
	MoveToCamera,
	MoveToWorldCenter
};

DECLARE_DELEGATE_OneParam(FOnPaletteButtonPressed,EPaletteButtonType);

class UODPaletteDataObject;
class UWrapBoxSlot;
class SImage;

UCLASS()
class OBJECTDISTRIBUTION_API UODPaletteSlate : public UPanelWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UODToolSubsystem> ToolSubsystem;

public:
	FORCEINLINE TObjectPtr<UODToolSubsystem> GetToolSubsystem();
	
	UODPaletteSlate(const FObjectInitializer& ObjectInitializer);
	
	static bool IsInPie();

	/** The inner slot padding goes between slots sharing borders */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Content Layout")
	FVector2D InnerSlotPadding;

	/** When this size is exceeded, elements will start appearing on the next line. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout", meta=(EditCondition = "bExplicitWrapSize"))
	float WrapSize;

	/** Use explicit wrap size whenever possible. It greatly simplifies layout calculations and reduces likelihood of "wiggling UI" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout")
	bool bExplicitWrapSize;

	/** The alignment of each line of wrapped content. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	/** Determines if the Wrap Box should arranges the widgets left-to-right or top-to-bottom */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout")
	TEnumAsByte<EOrientation> Orientation = EOrientation::Orient_Horizontal;

	/** Sets the inner slot padding goes between slots sharing borders */
	UFUNCTION(BlueprintCallable, Category="Content Layout")
	void SetInnerSlotPadding(FVector2D InPadding);

	UFUNCTION(BlueprintCallable, Category="Content Layout")
	void SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment);

	UFUNCTION(BlueprintCallable, Category="Panel")
	UWrapBoxSlot* AddChildToWrapBox(UWidget* Content);

#if WITH_EDITOR
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

protected:

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

	TSharedPtr<SWrapBox> MyWrapBox;
	TSharedPtr<class SNumericEntryBox<float>> SpawnDensityBox;
	TSharedPtr<class STextBlock> TotalSpawnCount;
	TSharedPtr<class STextComboBox> FinishingTypeComboBox;
	TSharedPtr<class STextComboBox> TargetTypeComboBox;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	TSharedRef<SWidget> BuildCreationToolbar() const;

	TSharedRef<SWidget> BuildSpawnCenterToolbar() const;

public:
	FOnPaletteButtonPressed OnPaletteButtonPressed;
private:
	UPROPERTY()
	TObjectPtr<UODPaletteDataObject> PaletteDataObject;

	UPROPERTY()
	TObjectPtr<UDetailsView> PaletteObjectDataDetails;

	UPROPERTY()
	TObjectPtr<UDetailsView> PropDistributionDetails;

public:
	TObjectPtr<UODPaletteDataObject> GetPaletteDataObject() const {return PaletteDataObject;}
	TObjectPtr<UDetailsView> GetPaletteObjectDataDetails() const {return PaletteObjectDataDetails;}
	TObjectPtr<UDetailsView> GetPropDistributionDetails() const {return PropDistributionDetails;}
	
private:
	float SpawnDensity = 0.5f;
	
	void SpawnDensityChanged(float InDensity);
	bool IsEnabled_SpawnDensity() const;
	TOptional<float> GetSpawnDensityValue() const;
	
	void SetCollisionTestCheckState(const bool InNewState);
	bool GetCollisionTestCheckState();
	void OnCollisionTestChanged(int32 InNewValue);
	bool IsEnabled_CollisionTest() const;
	TOptional<int32> GetCollisionTestValue() const;

	void OnKillZChanged(float InNewValue);
	TOptional<float> GetKillZValue() const;
	
	//ToolWindow Interface
public:
	void OnObjectSelectionChanged();
	void OnTotalSpawnCountChanged(const float& InSpawnCount);
	void OnDistributionRegenerated(const int32& InCollidingObjects) const;
	
private:
	TArray<TSharedPtr<FString>> FinishingTypeNames;
	TArray<TSharedPtr<FString>> TargetTypeNames;
	
	TSharedPtr<FString> GetInitialFinishingTypeName();
	TSharedPtr<FString> GetInitialTargetTypeName();
	
	bool OnAreAssetsValidForDrop(TArrayView<FAssetData> DraggedAssets) const;
	void HandleAssetsDropped(const FDragDropEvent& DragDropEvent, TArrayView<FAssetData> DraggedAssets) const;
	
public:
	FOnAssetDroppedSignature OnAssetsDropped;
	FOnPaletteBackgroundPressedSignature OnPaletteBackgroundPressedSignature;
	

	void OnResetParameters();
};


