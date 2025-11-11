// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spline/ASAutoSplineBase.h"
#include "ASCustomClassSpline.generated.h"

class USceneComponent;
class UChildActorComponent;

UENUM(BlueprintType, Category = "Auto Plugin Tool Settings")
enum class ESplineClassInstType : uint8
{
	Component		UMETA(DisplayName = "Component"),
	Actor     	    UMETA(DisplayName = "Actor"),
};


UCLASS()
class AUTOSPLINE_API AASCustomClassSpline : public AASAutoSplineBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	FRandomStream RandomStream;

	UPROPERTY()
	TArray<TObjectPtr<USceneComponent>> DynamicSplineObjects;
	
public:	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Instances",meta=(NoResetToDefault))
	ESplineClassInstType InstanceType = ESplineClassInstType::Component;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(EditCondition="InstanceType == ESplineClassInstType::Actor",EditConditionHides),Category = "Tool Settings|Instances")
	TArray<TSubclassOf<AActor>> SplineActorClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(EditCondition="InstanceType == ESplineClassInstType::Component",EditConditionHides),Category = "Tool Settings|Instances")
	TArray<TSubclassOf<USceneComponent>> SplineComponentClasses;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1), Category = "Tool Settings|Distribution")
	int32 NumberOfInstance = 1;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Distribution",DisplayName="Distribute Randomly",meta=(ToolTip = "Selects actors or components randomly from the list"))
	bool bSelectRandom = false;

	UPROPERTY(EditAnywhere, Category = "Tool Settings|Distribution",meta=(ToolTip = "Initially adds local rotation offset."))
	FRotator InitialRotationOffset = FRotator(0.0f,0.0f,0.0f);

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bRandomPositionOffset = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bRandomPositionOffset",ClampMin = 0.0f), Category = "Tool Settings|Distribution")
	FVector RandomPositionOffset = FVector(0,250.0f,0);
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bRandomRotationOffset = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bRandomRotationOffset",ClampMin = 0, ClampMax = 180.0f), Category = "Tool Settings|Distribution")
	FRotator RandomRotationOffset = FRotator(0.0f,90.0f,0.0f);

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bInnerSplineOffset = false;
	
	UPROPERTY(EditAnywhere ,meta=(EditCondition="bInnerSplineOffset",ClampMin = 0,NoSpinbox), Category = "Tool Settings|Distribution",DisplayName="Inner Spline Offset Range")
	FVector2D InnerSplineOffset = FVector2D(250.0f,250.0f);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Distribution",meta=(ToolTip = "Stream value for random actions",NoResetToDefault))
	ESplineObjectRotationType RotationType = ESplineObjectRotationType::FollowSpline;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tool Settings|Distribution",meta=(ToolTip = "Stream value for random actions",NoResetToDefault))
	bool SnapToSurface = false;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Tool Settings|Distribution")
	bool bRandomZ = false;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bRandomZ", ClampMin = 0,ClampMax = 360.0f),DisplayName = "Random Yaw Rotation Rate" ,Category = "Tool Settings|Distribution",meta=(ToolTip = "Randomizes the rotation of the mesh on the yaw axis by the entered value",NoResetToDefault))
	float  RandomRotationRate  = 45.0f;

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
	
private:
	void CreateSpline();

	UChildActorComponent* CreateChildActorComponent(TSubclassOf<AActor> InClass);
	USceneComponent* CreateComponentWithClass(TSubclassOf<USceneComponent> InClass);

	void ResetFormerSpline();

protected:
	virtual void RedesignTheSplineElements() override;

	virtual FString GetAsASplineLabel() override {return FString(TEXT("Custom Class Spline"));}

private:
	void CheckForClosedStatus();
	
	void AddRandomPositionOffset(FVector& InVector) const;
	void AddRandomRotationOffset(FRotator& InRotator) const;

	void CheckForSurfaceSnapping() const;

public:
#if WITH_EDITOR
	
	class FReply OnIsolatePressed();

private:
	virtual void CreateActorsFromSpline(bool bInSelectAfter = false) override;

	static TObjectPtr<AActor> SpawnAndGetAnActor(const FTransform& InTransform, const FString& InActorLabel,UWorld* InWorld);

#endif


	
};

