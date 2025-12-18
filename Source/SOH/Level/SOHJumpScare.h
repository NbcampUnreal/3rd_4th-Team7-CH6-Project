#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHJumpScare.generated.h"

class UBoxComponent;
class UArrowComponent;
class USoundBase;
class UAnimMontage;
class UAudioComponent;
class ACharacter;

UCLASS()
class SOH_API ASOHJumpScare : public AActor
{
	GENERATED_BODY()

public:
	ASOHJumpScare();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* SpawnBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* RemoveBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* SpawnArrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Spawn")
	TSubclassOf<ACharacter> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Spawn")
	USoundBase* SpawnSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Spawn")
	bool bSpawnSFXLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action")
	UAnimMontage* MontageToPlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action")
	USoundBase* ActionSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action")
	bool bStopSpawnSFXOnRemove = true;

	// 플레이어에게 날아가기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action")
	bool bLaunchTowardPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action", meta = (EditCondition = "bLaunchTowardPlayer"))
	float LaunchStrength = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action", meta = (EditCondition = "bLaunchTowardPlayer"))
	float LaunchUpStrength = 150.f;

	// 한번만 동작
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Flags")
	bool bSpawnOnlyOnce = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Flags")
	bool bRemoveOnlyOnce = true;

	// Runtime
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runtime")
	ACharacter* SpawnedCharacter = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runtime")
	UAudioComponent* SpawnAudioComp = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runtime")
	UAudioComponent* ActionAudioComp = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runtime")
	bool bSpawnTriggered = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runtime")
	bool bRemoveTriggered = false;

	// Overlap
	UFUNCTION()
	void OnSpawnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnRemoveBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// Montage end
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Helpers
	bool IsPlayer(AActor* OtherActor) const;
	ACharacter* GetPlayerChar() const;

	void SpawnCharacter();
	void TriggerRemoveAction();

	void StartSpawnSFXAttached();
	void StopSpawnSFX();

	void PlayActionSFXAttached();
	void CleanupAudioComps();
	void DestroySpawned();
};