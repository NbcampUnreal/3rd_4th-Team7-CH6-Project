// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/MBBuildingManagerInterface.h"
#include "Engine/EngineTypes.h"
#include "Data/MBDataStructs.h"
#include "Tickable.h"
#include "Engine/HitResult.h"
#include "MBBuildingManager.generated.h"

#pragma  region ForwardDeclerations
class UInstancedStaticMeshComponent;
enum class EBuildingCategory : uint8;
enum class EPlacementStatus : uint8;
enum class EMBAxis : uint8;
class UMBToolSubsystem;
class UDataTable;
class UMaterialInstanceDynamic;
struct FModularBuildingAssetData;
class UEditorActorSubsystem;
#pragma  endregion ForwardDeclerations


USTRUCT()
struct FMultiPlacementData
{
	GENERATED_BODY()
	
	UPROPERTY()
	EMBAxis Axis = EMBAxis::None;
	
	UPROPERTY()
	float	HalfLength = 0.0f;

	UPROPERTY()
	int32	SpawnCount = 0;

	UPROPERTY()
	FVector	Direction = FVector::ZeroVector;
};

UCLASS()
class MODULARBUILDING_API UMBBuildingManager : public UObject,public FTickableGameObject, public IMBBuildingManagerInterface
{
	GENERATED_BODY()
	

#pragma  region PermanentReferences
	
	UPROPERTY()
	TObjectPtr<UMBToolSubsystem> ToolSettings;

	UPROPERTY()
	TObjectPtr<AActor> MovedActor;

	UPROPERTY()
	TObjectPtr<AActor> HitActor;
	
	UPROPERTY()
	TObjectPtr<AActor> OverlappedActor;

#pragma  endregion PermanentReferences

#pragma region TickSetup

public:
	virtual void Tick( float DeltaTime ) override;
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
	}
	virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}
	virtual bool IsTickableInEditor() const override
	{
		return true;
	}

	virtual bool IsAllowedToTick() const override {return bCanTick;}

	private:
	bool bCanTick = true;
	
	uint32 LastFrameNumberWeTicked = INDEX_NONE;

#pragma endregion TickSetup

#pragma  region BuildingVariables
private:
	
	UPROPERTY()
	EBuildingCategory HitCategory;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	FRotator PropTargetRotation;
	
	FHitResult GlobalHitResult;

	UPROPERTY()
	EPlacementStatus PlacementStatus;

	bool bThirdAxisPositionState;

	bool bSecondAxisPositionState;

	bool bRepositionMainAxis;

public:
	void ToggleThirdAxisPositionState() {bThirdAxisPositionState = !bThirdAxisPositionState;}
	void ToggleSecondAxisPositionState() {bSecondAxisPositionState = !bSecondAxisPositionState;}
	void ToggleRepositionMainAxis() {bRepositionMainAxis = !bRepositionMainAxis;}

#pragma  endregion BuildingVariables

#pragma region ModularBuildingVariables

public:
	bool& GetIsPlacementInProgress(){return bPlacementInProgress;}
	
private:
	bool bPlacementInProgress;

	
#pragma  endregion  ModularBuildingVariables

#pragma region ModularBuilding

	void FindAndPlaceModular();
	
	EMBAxis FilterCancelledAxis() const;
	
	bool IsTheCursorCenterOfTheMesh(const EMBAxis& BlockedAxis,const FVector& InHitWorldOrigin) const;
	
	void CalculateAndSetModularTargetLocation(const EMBAxis& InWorkingAxis,const bool& InWorkingDir,const EMBAxis& InBlockedAxis);

#pragma endregion ModularBuilding
	
#pragma  region Initalize&Update
	
public:
	void InitializeManager();
	
#pragma  endregion Initalize&Update

#pragma  region BuildingFunctions

public:
	virtual void ResetPropSnapRotation() override;

private:
	void FindAndPlaceFree();

	void PlaceInTheFieldOfView();

	//Adjacent Check
	void CheckForAdjacent();
	void TraceOnSixDirection();

	UPROPERTY()
	EMBAxis LockedAxis;

	UPROPERTY()
	float LockedAxisValue;

	UPROPERTY()
	EMBAxis CancelledWorldAxis;

	UPROPERTY()
	float CancelledValue;

	
	void LockAxisOnFreePlacement();
	
	void SetModActorRotation() const;
	virtual void ApplyScaleRate() override;
	float PropScaleRate = 1;
	
	FVector GenerateModActorSpawnLocation();

	TObjectPtr<AActor> CreateModActor(const FString& AssetName,const FVector& InLocation,const FRotator& InRotation,const EBuildingCategory InCategory) const;

	static void GetRidOfFractals(FVector& InLocation);
public:
	void CreateAssetAndStartPlacementProgress(const FString& AssetName);

	EBuildingCategory GetBuildingCategoryWithAssetName(const FString& InAssetName) const;

private:
	void DestroyMovedActor();
	
	void SetModActorPlacementStatus(const EPlacementStatus InNewPlacementStatus);
	
	void ChangePlacementMode(const EBuildingCategory& InNewBuildingCategory) const;

	void CheckForSpotAvailability();

	void DoTrace(FHitResult& OutResult) const;
	
	FORCEINLINE bool IsGridEnabled() const;

	void PlaceTheMovedActor(const bool& bRegenerate);
	
	FORCEINLINE FName GetModularType(const AActor* InModActor) const;

	UFUNCTION()
	void OnPlacementTypeChanged();

public:
	void CancelPlacement();


#pragma  endregion BuildingFunctions
	
#pragma  region AssetGetters

public:
	TArray<FModularBuildingAssetData*> GetModularAssetData() const;
	
#pragma  endregion AssetGetters

#pragma region Inputs
public:
	bool LeftClickPressed();

	void ResetRegularVariablesAfterPlacement();

	void RotateActor(bool bInClockwise);
	
	bool EscPressed();
	
	bool AltPressed(bool bIsPressed);

private:
	bool bReplacementModeActivated;

#pragma endregion Inputs

#pragma  region  DynamicMaterial

protected:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PlacementDynamicMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PlacementDynamicMaterialForNanite;

#pragma  endregion  DynamicMaterial

#pragma region Duplication
private:
	bool bIsDuplicatingNow;

	UPROPERTY()
	TObjectPtr<AActor> SelectedDupActor;
	
	UPROPERTY()
	TArray<AActor*> Placeholders;
	
	UPROPERTY()
	TArray<AActor*> DuplicatedFreePool;

	UPROPERTY()
	FDuplicationData XDupData = FDuplicationData(EMBAxis::AxisX,1,0.f,true);

	UPROPERTY()
	FDuplicationData YDupData = FDuplicationData(EMBAxis::AxisY,1,0.f,true);

	UPROPERTY()
	FDuplicationData ZDupData = FDuplicationData(EMBAxis::AxisZ,1,0.f,true);

	UPROPERTY()
	FDuplicationFilters DuplicationFilters = FDuplicationFilters(false,false);
	
public:
	
	
	const bool & GetIsDuplicationNow() const {return bIsDuplicatingNow;}

	void SetNewDuplicationDataInBm(const FDuplicationData& InDuplicationData);

private:
	void SetReceivedDuplicationData(const FDuplicationData& InDuplicationData);
	
	void RegenerateTheDuplication();

public:
	void StopModularDuplication(bool bInApplyChanges);

	bool HasTheDuplicationReset() const {return Placeholders.IsEmpty();}

	virtual void ResetModularDuplication() override;

private:
	

	FORCEINLINE bool FilterXDuplicationForHole(const uint16& InIndex,const uint16& InNum) const;
	FORCEINLINE bool FilterYDuplicationForHole(const uint16& InIndex,const uint16& InNum) const;

	FORCEINLINE bool FilterDuplicationForRectangle() const;

	void MakeDuplicationRectangle();
	
	void MoveAllObjectToThePool();

	void DeletePoolAndResetVariables();
	
	TObjectPtr<AActor> GetDupActorFromPool();

	TArray<AActor*> SpawnDupPlaceHolders(const int32& InNum,const FVector& InDir, const float& InOffset,AActor* ActorToFollow,UEditorActorSubsystem* InEditorActorSubsystem,EMBAxis DupAxis,bool bCanSpawn);
public:
	void ApplyModularDuplicationFilterInBM(const FDuplicationFilters& InDuplicationFilters);
	FDuplicationFilters* GetExistingDuplicationFilterInBM() {return &DuplicationFilters;}
	
	TArray<FDuplicationData*> GetDuplicationData();
	
	bool IsTheActorFromDuplicationPool(AActor* InActor) const;
	
	void SelectTheDuplicationActor() const;

	virtual void ApplyMaterialToDuplicatedActors(const int32& InSlotIndex, UMaterialInterface* InMaterialInterface) override;

#pragma endregion Duplication

#pragma region MultiplePlacement

public:
	void PlaceMultiple();
	
private:
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> MultiPlacementBaseComponent;

	void UpdateMultiPlacement()const;

	void TraceToSurface(const FVector& InTraceOrigin,const FVector& InTraceEnd, FVector& InTargetLocation) const;
	void CheckForAnotherModularObjectInSameSpot(AActor* InActor) const;
	void CalculateMultiActorCountForFirstAxis(const EMBAxis& InAxis,FVector& Direction,int32& OutCount,float& OutHalfLength,const FVector& TargetPoint) const;
	void CalculateMultiActorCountForSecondAxis(const EMBAxis& InAxis,FVector& Direction,int32& OutCount,float& OutHalfLength,const FVector& TargetPoint) const;

	static int32 CalculateTotalMultiSpawnCount(const int32& InFirstCount, const int32& InSecondCount);
	
	void CreateMultiPlacementActorNeed(int32 InCount) const;
	
	void PlaceMultipleObjects(const FMultiPlacementData& InFirstMultiData,const FMultiPlacementData& InSecondMultiData) const;

	void GetNearestHitAxisAndDirection(FVector& OutDirection,EMBAxis& OutAxis) const;
	static void CalculateFirstAndSecondMultiPlacementAxis(EMBAxis& OutFirstAxis,EMBAxis& OutSecondAxis);
	bool CalculateDirectionOfAxis(const EMBAxis& InFirstAxis) const;

	void CancelSinglePlacement();
	void CancelMultiPlacement();
	
#pragma endregion MultiplePlacement

#pragma region ModularMeshOperations
	
	FORCEINLINE virtual UStaticMeshComponent* GetMMComponent(const AActor* InActor) const;

	FVector GetFixedMMLocation(const AActor* InActor) const;
	
	FORCEINLINE virtual FString GetMMAssetName(const AActor* InActor) const;

	FORCEINLINE virtual FVector GetMMLocalExtent(const AActor* InActor) const;
	
	FORCEINLINE virtual FVector GetMMWorldExtent(const AActor* InActor) const;
	
	FORCEINLINE virtual FVector GetMMWorldOrigin(const AActor* InActor) const;

	FORCEINLINE virtual bool GetMMMeshAxisStatus(const AActor* InActor,const EMBAxis InAxis) const;

	FORCEINLINE virtual bool IsMMMeshOnCenterOfAxis(const AActor* InActor,const EMBAxis& InAxis) const;
	
	FORCEINLINE virtual float GetMMMeshLengthInDirectionAndAxis(const AActor* InActor,const EMBAxis& InAxis,const bool& InWorkingDir)const;
	
	FORCEINLINE virtual float GetMMHalfLengthOfAxis(const AActor* InActor, const EMBAxis& InAxis) const;
	
#pragma endregion ModularMeshOperations

#pragma region ShuttingDown
	
public:
	void ShutDownBuildingManager();
	
	virtual void BeginDestroy() override;

#pragma endregion ShuttingDown
};
