// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

/** Physical Layout Edit Mode */
class FODSimulationMode : public FEdMode
{
public:
	const static FEditorModeID OD_SimulationModeID;
	
	/** Constructor */
	FODSimulationMode();

	/** FEdMode interface */
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void ActorSelectionChangeNotify() override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;
	virtual bool StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport);
	virtual bool EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport);
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;
	virtual bool ProcessCapturedMouseMoves(FEditorViewportClient* InViewportClient, FViewport* InViewport, const TArrayView<FIntPoint>& CapturedMouseMoves) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual bool ShowModeWidgets() const;
	virtual bool UsesTransformWidget() const;
	virtual FVector GetWidgetLocation() const override;
	bool UsesToolkits() const override;
};