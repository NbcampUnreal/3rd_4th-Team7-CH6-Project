// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ASToolWindow.generated.h"

class UASSplineSlot;
class UContentWidget;
class AASAutoSplineBase;
class UButton;

UENUM(BlueprintType, Category = "Auto Spline")
enum class EAutoSplineEditorType: uint8
{
	None				 UMETA(DisplayName = "None"),
	Deformable			 UMETA(DisplayName = "Deformable Static Mesh"),
	SMDistribution		 UMETA(DisplayName = "Static Mesh Distribution"),
	CCDistribution       UMETA(DisplayName = "Custom Class Distribution"),
};

UCLASS()
class AUTOSPLINEEDITOR_API UASToolWindow : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<TObjectPtr<UASSplineSlot>> SplineSlots;
	
	EAutoSplineEditorType ActiveViewListType; 

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

#pragma region UI
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeformableBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MeshDistributionBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClassDistributionBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ViewDeformableBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ViewMeshDistBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ViewClassDistBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UContentWidget> SplineContentBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ViewList;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCheckBox> SelectAllCheckbox;
	
	UFUNCTION()
	void DeformableBtnPressed();
	
	UFUNCTION()
	void MeshDistributionBtnPressed();

	UFUNCTION()
	void ClassDistributionBtnPressed();
	
	UFUNCTION()
	void ViewDeformableBtnPressed();
	
	UFUNCTION()
	void ViewMeshDistBtnPressed();

	UFUNCTION()
	void ViewClassDistBtnPressed();

		UFUNCTION()
	void SelectBtnPressed();

	UFUNCTION()
	void CreateBtnPressed();
	
	UFUNCTION()
	void SelectAllCheckboxChanged(bool InNewState);

#pragma endregion UI

private:

	void HandleOnLevelActorDeletedNative(AActor* InActor);

	void EjectSplineContentBox();

	void CheckForViewListButtonsAvailability();

	AActor* SpawnSplineActor(TSubclassOf<AASAutoSplineBase> InSplineClass) const;

	static FVector FindSpawnLocationForSpline();

	void OnSplineSlotCheckboxStateChanged(bool InState);

	TArray<AActor*> GetCheckedSplineActors();

	void AddNewCreatedItemToTheSplineList(const EAutoSplineEditorType InCreatedType,const AActor* InSplineActor);

	void CreateSplineSlotAndAddActorToTheList(UWorld* InWorld, const AActor* InSplineActor);
	
	static TArray<AActor*> GetAllActorsWithGivenObjectNames(const TArray<FString>& InNames);
};

