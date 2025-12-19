#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHJumpScare.generated.h"

class UBoxComponent;
class UArrowComponent;
class UAudioComponent;
class USoundBase;
class UAnimMontage;
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

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SpawnBox;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* RemoveBox;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* SpawnArrow;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Spawn")
	TSubclassOf<ACharacter> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Spawn")
	bool bSpawnOnlyOnce = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Remove")
	bool bRemoveOnlyOnce = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|SFX")
	USoundBase* SpawnSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|SFX")
	bool bSpawnSFXLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|SFX")
	bool bStopSpawnSFXOnRemove = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|SFX")
	USoundBase* ActionSFX = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action")
	UAnimMontage* MontageToPlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Action")
	bool bLaunchTowardPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Dash", meta = (EditCondition = "bLaunchTowardPlayer", ClampMin = "0.0"))
	float DashSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Dash", meta = (EditCondition = "bLaunchTowardPlayer", ClampMin = "0.0"))
	float DashDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Dash", meta = (EditCondition = "bLaunchTowardPlayer", ClampMin = "0.005"))
	float DashTickInterval = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Lifetime", meta = (ClampMin = "0.0"))
	float JumpScareLifetime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Lifetime")
	bool bDestroyOnMontageEnd = false;

protected:
	UPROPERTY()
	ACharacter* SpawnedCharacter = nullptr;

	UPROPERTY()
	UAudioComponent* SpawnAudioComp = nullptr;

	UPROPERTY()
	UAudioComponent* ActionAudioComp = nullptr;

	bool bSpawnTriggered = false;
	bool bRemoveTriggered = false;

	FTimerHandle DashTimerHandle;
	FTimerHandle LifetimeTimerHandle;
	FTimerHandle MontageTimerHandle;

protected:
	UFUNCTION()
	void OnSpawnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRemoveBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	bool IsPlayer(AActor* OtherActor) const;
	ACharacter* GetPlayerChar() const;

	void SpawnCharacter();
	void StartSpawnSFXAttached();
	void StopSpawnSFX();
	void PlayActionSFXAttached();
	void TriggerRemoveAction();
	void CleanupAudioComps();
	void DestroySpawned();
};