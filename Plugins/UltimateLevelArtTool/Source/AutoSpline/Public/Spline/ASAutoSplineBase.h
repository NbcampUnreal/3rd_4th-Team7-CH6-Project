// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASAutoSplineBase.generated.h"

class UASSplineComponent;
class USplineComponent;

UENUM(BlueprintType, Category = "Auto Plugin Tool Settings")
enum class ESplineObjectRotationType : uint8
{
	Keep					UMETA(DisplayName = "Keep Rotation"),
	FollowSpline			UMETA(DisplayName = "Follow The Spline"),
	FollowSplineAndSurface	UMETA(DisplayName = "Follow The Spline & Surface")
};

UENUM(BlueprintType, Category = "Auto Plugin Tool Settings")
enum class EASMeshAxis : uint8
{
	AxisX			UMETA(DisplayName = "X"),
	AxisY			UMETA(DisplayName = "Y"),
	AxisZ			UMETA(DisplayName = "Z"),
};

UCLASS(Abstract)
class AUTOSPLINE_API AASAutoSplineBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UASSplineComponent> SplineComponent;

public:
	AASAutoSplineBase();

	virtual void OnConstruction(const FTransform& Transform) override;
	
	USplineComponent* GetSplineComp() const;

protected:
	virtual float GetMeshLength(const UStaticMesh* InStaticMesh,EASMeshAxis InMeshAxis);
	virtual float GetComponentLength(const USceneComponent* InSceneComponent);
	virtual float GetActorLength(const AActor* InActor);

	static FVector GetSurfaceNormalAtLocation(const FVector& InLocation);
	void SnapLocationToSurfaceIfAvailableOne(FVector& InLocation) const;

public:
	void AdjustSplineLabel();

	virtual void CreateActorsFromSpline(bool bInSelectAfter = false);
	
protected:
	
	virtual FString GetAsASplineLabel() {return FString(TEXT("Auto Spline Base"));}

public:
	virtual void RedesignTheSplineElements();

	
#if WITH_EDITOR
	void OnSplineUpdated();
	
	void CopyTargetSpline(USplineComponent* InSplineComponent) const;
#endif
	
	

	
};
