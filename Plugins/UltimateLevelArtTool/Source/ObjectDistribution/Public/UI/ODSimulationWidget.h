// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "ODSimulationWidget.generated.h"

DECLARE_DELEGATE(FOnStartSimClicked);
DECLARE_DELEGATE(FOnStopSimClicked);

class SButton;
class SImage;

UCLASS()
class OBJECTDISTRIBUTION_API UODSimulationWidget : public UWidget
{
	GENERATED_BODY()


public:
	virtual void SynchronizeProperties() override;
	
	void SimulationFinished();
	void OnHandleBeginPie();
	void OnHandlePausePie();
	void OnHandleResumePie();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

public:
	void ResetSimulationInterface();
	
public:
	FOnStartSimClicked OnStartSimClicked;
	FOnStopSimClicked OnStopSimClicked;
	
	TSharedPtr<SButton> StartButton;
	TSharedPtr<SButton> StopButton;
	TSharedPtr<SImage> StartImage;

private:
	FReply OnStartBtnClicked();
	FReply OnStopBtnClicked();
	FReply OnAdvanceFrameButtonClicked();

	static bool IsInPie();
};
