// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODSimulationWidget.h"
#include "Editor.h"
#include "ODToolStyle.h"
#include "ODToolSubsystem.h"
#include "Components/HorizontalBox.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "ODSimulationWidget"

void UODSimulationWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

TSharedRef<SWidget> UODSimulationWidget::RebuildWidget()
{
	const FMargin StandardLeftPadding(6.f, 0.f, 0.f, 0.f);
	
	//Start Button
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(0)
	.HAlign(HAlign_Left)
	[
		SNew(SBox)
		.WidthOverride(24)
		.HeightOverride(24)
		[
			//Start Button
			SAssignNew(StartButton,SButton)
			.ButtonStyle( &FODToolStyle::GetCreatedToolSlateStyleSet()->GetWidgetStyle<FButtonStyle>(FName("ObjectDistribution.ODSimButtonStyle")))
			.ContentPadding(FMargin(0))
			.ToolTipText_Lambda([]()
			{
				if(IsInPie() && GEditor->PlayWorld)
				{
					if(GEditor->PlayWorld->IsPaused())
					{
						return LOCTEXT("StartSimToolTipText", "Resume Simulation");
					}
					return LOCTEXT("StartSimToolTipText", "Pause Simulation");
				}
				return LOCTEXT("StartSimulationToolTipText", "Start Simulation");
			})
			
			.OnClicked_UObject(this,&UODSimulationWidget::OnStartBtnClicked)
			[
				SAssignNew(StartImage,SImage)

				.Image_Lambda([]()
				{
					if(IsInPie() && GEditor->PlayWorld)
					{
						if(GEditor->PlayWorld->IsPaused())
						{
							return FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPauseButtonIcon").GetIcon();
						}
						return FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon();
					}
					return FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon();
				})
				
			]
		]
	]
	
	//Stop Button
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(StandardLeftPadding)
	.HAlign(HAlign_Left)
	[
		SNew(SBox)
		.WidthOverride(24)
		.HeightOverride(24)
		[
			//Stop Button
			SAssignNew(StopButton,SButton)
			.ButtonStyle( &FODToolStyle::GetCreatedToolSlateStyleSet()->GetWidgetStyle<FButtonStyle>(FName("ObjectDistribution.ODSimButtonStyle")))
			.ContentPadding(FMargin(0))
			.ToolTipText(LOCTEXT("StopSimulationToolTipText", "Stop Simulation"))
			.IsEnabled_Lambda([]()
			{
				const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
				if(!ToolSubsystem){return false;}

				return ToolSubsystem->bIsSimulationInProgress;
			})
			.OnClicked_UObject(this,&UODSimulationWidget::OnStopBtnClicked)
			[
				SNew(SImage)
				.Image(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODStopButtonIcon").GetIcon())
			]
		]
	];
}

void UODSimulationWidget::ResetSimulationInterface()
{
	if(!StartImage.IsValid() || !StartButton.IsValid() || !StopButton.IsValid()){return;}
	
	StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon());
	StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Start Simulation"));
	StopButton->SetEnabled(false);
	
}

FReply UODSimulationWidget::OnStartBtnClicked()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return FReply::Handled();}
	
	if(!ToolSubsystem->bIsSimulationInProgress)
	{
		if(ToolSubsystem->CreatedDistObjects.Num() == 0)
		{
			return FReply::Handled();
		}
	}
	
	OnStartSimClicked.ExecuteIfBound();
	
	StopButton->SetEnabled(true);
	
	if(ToolSubsystem->bIsSimulationInProgress)
	{
		if(ToolSubsystem->bIsSimulating)
		{
			StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPauseButtonIcon").GetIcon());
			StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Pause Simulation"));
		}
		else
		{
			StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon());
			StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Resume Simulation"));
		}
	}
	
	return FReply::Handled();
}

void UODSimulationWidget::SimulationFinished()
{
	if(!StartImage.IsValid() || !StartButton.IsValid() || !StopButton.IsValid()){return;}
	
	StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon());
	StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Start Simulation"));
	StopButton->SetEnabled(false);
}

void UODSimulationWidget::OnHandleBeginPie()
{
	if(!StartImage.IsValid() || !StartButton.IsValid() || !StopButton.IsValid()){return;}
	
	StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPauseButtonIcon").GetIcon());
	StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Pause Simulation"));
	StopButton->SetEnabled(true);
}

void UODSimulationWidget::OnHandlePausePie()
{
	if(!StartImage.IsValid() || !StartButton.IsValid()){return;}
	
	StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon());
	StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Resume Simulation"));
}

void UODSimulationWidget::OnHandleResumePie()
{
	if(!StartImage.IsValid() || !StartButton.IsValid() || !StopButton.IsValid()){return;}
	
	StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPauseButtonIcon").GetIcon());
	StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Pause Simulation"));
	StopButton->SetEnabled(true);
}

FReply UODSimulationWidget::OnStopBtnClicked()
{
	if(!StartImage.IsValid() || !StartButton.IsValid() || !StopButton.IsValid()){return FReply::Handled();}
	
	StartImage->SetImage(FSlateIcon(FODToolStyle::GetToolStyleName(),"ObjectDistribution.ODPlayButtonIcon").GetIcon());
	StartButton->SetToolTipText(LOCTEXT("StartSimToolTipText", "Start Simulation"));
	StopButton->SetEnabled(false);
	
	OnStopSimClicked.ExecuteIfBound();
	return FReply::Handled();
}


bool UODSimulationWidget::IsInPie()
{
	if (GEditor == nullptr){return true;}

	if(GEditor->GetPIEWorldContext())
	{
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE