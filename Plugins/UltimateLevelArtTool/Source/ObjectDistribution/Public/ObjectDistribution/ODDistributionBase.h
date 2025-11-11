// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ODMeshData.h"
#include "ODDistributionBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnODDistributionTypeChangedSignature, EObjectDistributionType);
DECLARE_DELEGATE_OneParam(FOnTotalSpawnCountChanged, float);
DECLARE_DELEGATE(FOnAfterODRegenerated);
DECLARE_DELEGATE_OneParam(FOnFinishConditionChangeSignature,bool);

class UODToolSubsystem;
class UODToolWindow;
class AStaticMeshActor;
class FReply;

UCLASS()
class OBJECTDISTRIBUTION_API UODDistributionBase : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UODToolWindow> ToolWindow;
	
	UPROPERTY()
	TObjectPtr<UODToolSubsystem> ToolSubsystem;

	FDistObjectData* FindDataFromMap(const FName& InIndexName) const;
	
	int32 TotalSpawnCount = 0;
	int32 CollidingObjects = 0;
	
	UPROPERTY()
	bool bIsInMixerMode = false;
	
	int32 CustomPresetDataNum;

public:
	FORCEINLINE int32& GetCollidingObjects() {return CollidingObjects;}
	
protected:
	TObjectPtr<UODToolSubsystem> GetToolSubsystem() const {return ToolSubsystem;}

public:
	FOnODDistributionTypeChangedSignature OnDistributionTypeChangedSignature;
	FOnAfterODRegenerated OnAfterODRegenerated;
	FOnFinishConditionChangeSignature OnFinishConditionChangeSignature;
	
	void Setup(UODToolWindow* InToolWindow);
	
	virtual void LoadDistData();

	virtual void BeginDestroy() override;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere,Category = "Distribution" , meta=(NoResetToDefault,DisplayPriority=1))
	EObjectDistributionType DistributionType = EObjectDistributionType::Cube;

public:
#if WITH_EDITOR

	//Creation
	void OnCreateDistributionPressed();
	void OnDeleteDistributionPressed();

	//Distribution
	void OnShuffleDistributionPressed();
	void OnSelectObjectsPressed();
	void OnFinishDistributionPressed();
	
	//Simulation
	void OnStartBtnPressed();
	void OnStopBtnPressed();
	void OnPauseSimulationPressed();
	void OnResumeSimulationPressed();
	


	//SpawnCenter
	void OnSelectSpawnCenterPressed();
	void OnMoveSpawnCenterToWorldOriginPressed();
	void OnMoveSpawnCenterToCameraPressed();
#endif

public:
	void NatureTick(const float& InDeltaTime);

private:
	void StopSimulationManually();
	
	void ResetSimulation();

	void CreatePaletteObjects();
	
	void DestroyObjects() const;

	bool  CheckForSpotSuitability(TObjectPtr<AStaticMeshActor> InSmActor) const;
	
	int32 GetInitialTotalCount() const;
public:
	void CalculateTotalSpawnCount();
	
	void ReDesignObjects();

	void AddSpawnCenterMotionDifferences(const FVector& InVelocity) const;
	void ReRotateObjectsOnSpawnCenter() const;

	void LevelActorDeleted(AActor* InActor);

	static void KeepSimulationChanges();

private:
	static TArray<AStaticMeshActor*> SpawnAndGetEmptyStaticMeshActors(const int32& InSpawnCount);

	static AStaticMeshActor* SpawnAndGetStaticMeshActor();

	FName GenerateUniquePresetPath(const AActor* InSampleActor) const;
	
	void FinishDistAsKeepActors() const;
	void FinishDistAsRunningMergeToll() const;

protected:
	virtual EObjectDistributionType GetDistributionType() {return EObjectDistributionType::Cube;}
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength);
	static FRotator CalculateRotation(const FVector& InLocation,const FVector& TargetLocation, const EObjectOrientation& Orientation);

public:
	FOnTotalSpawnCountChanged OnTotalSpawnCountChanged;

private:

#pragma region Physics
public:
	void SetSimulatePhysics() const;

public:
	FORCEINLINE bool IsInPie();
#pragma endregion Physics


#pragma region KillZ

private:
	UPROPERTY()
	TArray<FName> ActorsInKillZ;

	UPROPERTY()
	TArray<AActor*> SimulatedActors;

	bool bTraceForKillZ;
	float KillZCheckTimer;

	bool IsThereAnySimActor = true;
	
	void CollectAllSimulatedActors();
	void ReleaseSimulatedActorReferences();
	
	void CheckForKillZInSimulation();
	void DestroyKillZActors();
	
	void StartKillZCheck();
	void FinishKillZCheck();
	void CheckForActorsInKillZ();

public:
	void HandleBeginPIE();
	void HandleEndPIE();
	
#pragma endregion KillZ	

public:
	UFUNCTION()
	void OnPresetLoaded();

#pragma region MixerMode


private:

#pragma endregion  MixerMode
};


