// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ASSplineSlot.generated.h"

DECLARE_DELEGATE_OneParam(FOnSplineSlotCheckboxStateChanged, bool)

class UButton;
class UEditableTextBox;

UCLASS()
class AUTOSPLINEEDITOR_API UASSplineSlot : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString SplineObjectName;

	UPROPERTY()
	FString SplineObjectLabel;
	
public:
	const FString& GetSplineObjectName() const  {return SplineObjectName;}
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void SetSplineData(const FString& InLabel,const FString& InObjectName);
	
protected:
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> SplineNameTextBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SeeSplineBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RenameBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCheckBox> SelectionCheckbox;
	
	
	UFUNCTION()
	void SeeSplineBtnPressed();

	UFUNCTION()
	void RenameBtnPressed();
	
	UFUNCTION()
	void SplineNameTextBoxCommitted( const FText& InText, ETextCommit::Type InCommitMethod);

	UFUNCTION()
	void SelectionCheckboxChanged(bool InNewState);

public:
	FOnSplineSlotCheckboxStateChanged OnSplineSlotCheckboxStateChanged;

	void SetSelectionState(const bool& bInNewState) const;
	bool GetSelectionState() const;

private:
	static AActor* GetActorWithObjectName(const FString& InName);
};

