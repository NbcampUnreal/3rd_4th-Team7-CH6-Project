// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "ODSimulationMode.h"



#define LOCTEXT_NAMESPACE "FODSimulationMode"

const FEditorModeID FODSimulationMode::OD_SimulationModeID = TEXT("OD_SimulationModeID");

FODSimulationMode::FODSimulationMode()
{
	
}

void FODSimulationMode::Enter()
{
	UE_LOG(LogTemp,Warning,TEXT("Whener Wherever"));
	
	FEdMode::Enter();
}

void FODSimulationMode::Exit()
{

	
	FEdMode::Exit();
}

void FODSimulationMode::ActorSelectionChangeNotify()
{


	
	FEdMode::ActorSelectionChangeNotify();
}

void FODSimulationMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);
}

bool FODSimulationMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag,FRotator& InRot, FVector& InScale)
{
	
	return FEdMode::InputDelta(InViewportClient, InViewport, InDrag, InRot, InScale);
}

bool FODSimulationMode::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{

	
	return FEdMode::MouseMove(ViewportClient, Viewport, x, y);
}

bool FODSimulationMode::StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{

	
	GEditor->EndTransaction();
	GEditor->BeginTransaction(LOCTEXT("PhysicalLayoutMode_Transformation", "Transformation"));
	
	return FEdMode::StartTracking(InViewportClient, InViewport);
}

bool FODSimulationMode::EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{

	
	GEditor->EndTransaction();
	GEditor->NoteSelectionChange();
	
	return FEdMode::EndTracking(InViewportClient, InViewport);
}

bool FODSimulationMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key,EInputEvent Event)
{

	
	return FEdMode::InputKey(ViewportClient, Viewport, Key, Event);
}

bool FODSimulationMode::MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{

	
	return FEdMode::MouseEnter(ViewportClient, Viewport, x, y);
}

bool FODSimulationMode::ProcessCapturedMouseMoves(FEditorViewportClient* InViewportClient, FViewport* InViewport,const TArrayView<FIntPoint>& CapturedMouseMoves)
{

	
	return FEdMode::ProcessCapturedMouseMoves(InViewportClient, InViewport, CapturedMouseMoves);
}

void FODSimulationMode::AddReferencedObjects(FReferenceCollector& Collector)
{

	
	FEdMode::AddReferencedObjects(Collector);
}

bool FODSimulationMode::ShowModeWidgets() const
{

	
	return FEdMode::ShowModeWidgets();
}

bool FODSimulationMode::UsesTransformWidget() const
{

	
	return FEdMode::UsesTransformWidget();
}

FVector FODSimulationMode::GetWidgetLocation() const
{

	
	return FEdMode::GetWidgetLocation();
}

bool FODSimulationMode::UsesToolkits() const
{

	
	return FEdMode::UsesToolkits();
}


#undef LOCTEXT_NAMESPACE