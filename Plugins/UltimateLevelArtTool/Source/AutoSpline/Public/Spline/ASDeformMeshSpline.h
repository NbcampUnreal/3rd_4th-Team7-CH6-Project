// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ASAutoSplineBase.h"
#include "Components/SplineMeshComponent.h"
#include "ASDeformMeshSpline.generated.h"

UCLASS()
class AUTOSPLINE_API AASDeformMeshSpline : public AASAutoSplineBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshComponents;

	UPROPERTY()
	FRandomStream RandomStream;
	
public:	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings")
	TArray<TObjectPtr<UStaticMesh>> StaticMeshes;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings",DisplayName="Random Placement")
	bool bSelectRandom = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings")
	float OffsetOnSpline = 0.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings")
	EASMeshAxis ForwardAxis = EASMeshAxis::AxisX;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings",meta=(ClampMin = 0.01))
	float ForwardScale = 1.0f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings")
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::Type::NoCollision;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle),DisplayName="Auto Close The Spline" ,Category = "Tool Settings",AdvancedDisplay)
	bool bAutoCloseTheSpline = false;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bAutoCloseTheSpline",ToolTip = "The spline closes automatically when the first and last point are close enough"), Category = "Tool Settings",AdvancedDisplay)
	int32 AutoCloseTheSpline =  200;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings",meta=(ToolTip = "Stream value for random actions"),AdvancedDisplay)
	int32 Seed = 0;

#if WITH_EDITORONLY_DATA //Editor Only Data
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings",AdvancedDisplay)
	bool bCopyASpline = false;
	
	UPROPERTY(EditAnywhere, Category = "Tool Settings",meta=(EditCondition="bCopyASpline"),AdvancedDisplay)
	TObjectPtr<AASAutoSplineBase> CopyASpline;

#endif

protected:
	virtual void RedesignTheSplineElements() override;

	virtual FString GetAsASplineLabel() override {return FString(TEXT("Deform Mesh Spline"));}

private:

	FVector GetSplineUpVector() const;

	void CheckForSurfaceSnapping();

public:
#if WITH_EDITOR
	
	class FReply OnRunMergePressed();

	class FReply OnSnapPointsToSurface();

private:
	void CheckForClosedStatus() const;
	
	virtual void CreateActorsFromSpline(bool bInSelectAfter = false) override;

#endif

	
};
