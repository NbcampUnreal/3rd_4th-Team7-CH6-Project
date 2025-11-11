// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MBDataStructs.h"
#include "MBModularActorActionSettings.generated.h"

class UButton;
class UEditableTextBox;
class UCheckBox;
class UBorder;


UCLASS()
class MODULARBUILDING_API UMBModularActorActionSettings : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetDupBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> XDupText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> YDupText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ZDupText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> XDupOffset;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> YDupOffset;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ZDupOffset;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> XDirCheck;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> YDirCheck;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> ZDirCheck;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> HoleCheck;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> RectangleCheck;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> MaterialAssignmentBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UDetailsView> ModActorActionDetails;


public:
	UFUNCTION()
	void ApplyBtnPressed();
	
	UFUNCTION()
	void ResetDupBtnPressed();

	UFUNCTION()
	void OnXDupTextCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnYDupTextCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnZDupTextCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnXDupOffsetCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnYDupOffsetCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnZDupOffsetCommitted(const FText& Text,ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnXDirCheckChanged(bool bInNewCondition);

	UFUNCTION()
	void OnYDirCheckChanged(bool bInNewCondition);

	UFUNCTION()
	void OnZDirCheckChanged(bool bInNewCondition);

	UFUNCTION()
	void OnHoleCheckChanged(bool bInNewCondition);

	UFUNCTION()
	void OnRectangleCheckChanged(bool bInNewCondition);

	UPROPERTY()
	FDuplicationData XDuplicationData = FDuplicationData(EMBAxis::AxisX,1,0.0f,true);

	UPROPERTY()
	FDuplicationData YDuplicationData = FDuplicationData(EMBAxis::AxisY,1,0.0f,true);

	UPROPERTY()
	FDuplicationData ZDuplicationData = FDuplicationData(EMBAxis::AxisZ,1,0.0f,true);

	UPROPERTY()
	FDuplicationFilters DuplicationFilters = FDuplicationFilters(false,false);
	
	FORCEINLINE void CheckForNumOfDupCommit(FDuplicationData& InDuplicationData,const FText& InText, UEditableTextBox* INDupBox) const;
	FORCEINLINE void CheckForOffsetCommit(FDuplicationData& InDuplicationData,const FText& InText, UEditableTextBox* INDupBox) const;
	FORCEINLINE void CheckForDirCommit(FDuplicationData& InDuplicationData,const bool& bInNewBool) const;

	void RegenerateDuplicationMenu();
	void RegenerateDuplication(const FDuplicationData& InDuplicationData) const;
	

};
