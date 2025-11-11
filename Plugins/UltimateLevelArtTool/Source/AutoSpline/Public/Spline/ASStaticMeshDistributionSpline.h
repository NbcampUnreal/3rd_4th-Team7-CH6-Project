// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Spline/ASAutoSplineBase.h"
#include "ASStaticMeshDistributionSpline.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;

UENUM(BlueprintType, Category = "Modular Building Tool")
enum class ENonDeformInstanceType : uint8
{
	Sm			UMETA(DisplayName = "Static Mesh"),
	Ism     	UMETA(DisplayName = "Instanced Static Mesh"),
	HIsm		UMETA(DisplayName = "Hierarchical Instanced Static Mesh"),
};

UCLASS()
class AUTOSPLINE_API AASStaticMeshDistributionSpline : public AASAutoSplineBase
{
	GENERATED_BODY()

	UPROPERTY()
	FRandomStream RandomStream;

	void ResetFormerSpline();
	
	void CreateSplineMesh();

	UPROPERTY()
	TArray<TObjectPtr<UInstancedStaticMeshComponent>> DynamicSMInstanceComponents;
	
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> DynamicSMComponents;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;

	virtual void RedesignTheSplineElements() override;

protected:

#pragma region Details
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Instances",meta=(NoResetToDefault))
	ENonDeformInstanceType InstanceType = ENonDeformInstanceType::Ism;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Instances")
	TArray<TObjectPtr<UStaticMesh>> SplineStaticMeshes;

	UPROPERTY(EditAnywhere, Category = "Tool Settings|Instances",DisplayName="Custom End Mesh")
	bool bAddCustomEndMeshCond = false;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(EditCondition="bAddCustomEndMeshCond",EditConditionHides), Category = "Tool Settings|Instances")
	TObjectPtr<UStaticMesh> CustomEndMesh;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings")
	bool bSetNumber = false;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bSetNumber", ClampMin = 1), Category = "Tool Settings|Distribution")
	int32 NumberOfInstance = 1;

	UPROPERTY(EditAnywhere, meta=(EditCondition="!bSetNumber && NumberOfInstance >= 0",EditConditionHides), Category = "Tool Settings|Distribution")
	int32 OffsetOnSpline = 0;

	UPROPERTY(EditAnywhere, Category = "Tool Settings|Distribution",meta=(ToolTip = "Initially adds local rotation offset."))
	FRotator InitialRotationOffset = FRotator(0.0f,0.0f,0.0f);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Distribution",DisplayName="Distribute Randomly",meta=(ToolTip = "Selects static meshes randomly from the list"))
	bool bSelectRandom = false;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bRandomPositionOffset = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bRandomPositionOffset",ClampMin = 0.0f), Category = "Tool Settings|Distribution")
	FVector RandomPositionOffset = FVector(0,250.0f,0);
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bRandomRotationOffset = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bRandomRotationOffset",ClampMin = 0, ClampMax = 180.0f), Category = "Tool Settings|Distribution")
	FRotator RandomRotationOffset = FRotator(0.0f,90.0f,0.0f);

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bRandomScaleRange = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bRandomScaleRange",ClampMin = 0.01), Category = "Tool Settings|Distribution")
	FVector2D RandomScaleRange = FVector2D(1.0f,2.0f);

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bInnerSplineOffset = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bInnerSplineOffset",ClampMin = 0,NoSpinbox), Category = "Tool Settings|Distribution",DisplayName="Inner Spline Offset Range")
	FVector2D InnerSplineOffset = FVector2D(250.0f,250.0f);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Distribution",meta=(ToolTip = "Stream value for random actions",NoResetToDefault))
	ESplineObjectRotationType RotationType = ESplineObjectRotationType::FollowSpline;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Distribution",meta=(ToolTip = "Stream value for random actions",NoResetToDefault))
	bool SnapToSurface = false;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle),DisplayName="Auto Close The Spline" ,Category = "Tool Settings",AdvancedDisplay)
	bool bAutoCloseTheSpline = false;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bAutoCloseTheSpline",ToolTip = "The spline closes automatically when the first and last point are close enough"), Category = "Tool Settings",AdvancedDisplay)
	int32 AutoCloseTheSpline =  200;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings",meta=(ToolTip = "Stream value for random actions"),AdvancedDisplay)
	int32 Seed = 0;
	
	UPROPERTY(EditAnywhere, Category = "Tool Settings",AdvancedDisplay)
	TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Type::Movable;

#if WITH_EDITORONLY_DATA //Editor Only Data
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings",AdvancedDisplay)
	bool bCopyASpline = false;
	
	UPROPERTY(EditAnywhere, Category = "Tool Settings",meta=(EditCondition="bCopyASpline"),AdvancedDisplay)
	TObjectPtr<AASAutoSplineBase> CopyASpline;

#endif
	
	UPROPERTY(EditAnywhere, Category = "Tool Settings",AdvancedDisplay)
	EASMeshAxis ForwardAxis = EASMeshAxis::AxisX;
	
#pragma  endregion Details
	
	UStaticMeshComponent* CreateStaticMeshComponent(UStaticMesh* InStaticMesh,const FTransform& InRelativeTransform);
	UInstancedStaticMeshComponent* CreateInstancedStaticMeshComponent(const UClass* InISmClass, UStaticMesh* InStaticMesh);
	
	int32 GetMeshContainingInstancedSMCompIndex(UStaticMesh* InStaticMesh);

private:
	virtual FString GetAsASplineLabel() override {return FString(TEXT("Static Mesh Distribution Spline"));}
	
	void AddRandomPositionOffset(FVector& InVector) const;
	void AddRandomRotationOffset(FRotator& InRotator) const;
	float GetRandomScale() const;

	void CheckForSurfaceSnapping() const;
	
public:
#if WITH_EDITOR
	
	class FReply OnSetFreePressed();

	class FReply OnRunMergePressed();

protected:
	virtual void CreateActorsFromSpline(bool bInSelectAfter = false) override;

private:
	AActor* SpawnAndGetAnActor(const FTransform& InTransform, const FString& InActorLabel,UWorld* InWorld) const;

	void CheckForClosedStatus() const;
#endif
	
};
