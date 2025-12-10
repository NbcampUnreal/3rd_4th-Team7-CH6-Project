#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHAIMonster.generated.h"

class ATargetPoint;
class UAudioComponent;
class USoundBase;
class ASOHPatrolRoute;
class AAIController;

UCLASS()
class SOH_API ASOHAIMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHAIMonster();

protected:
	virtual void BeginPlay() override;

public:
	//두리번 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* LookAroundMontage;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayLookAroundMontage();

	//Patrol

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol", meta = (ExposeOnSpawn = "true"))
	ASOHPatrolRoute* PatrolRouteActor;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<ATargetPoint*> PatrolTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChaseSpeed;

	// 시야
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float SightRadius;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float LoseSightRadius;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float PeripheralVisionAngle;


	//청각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Hearing")
	float HearingRange;

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void SetMoveSpeed(float NewSpeed);

	//Attack

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	UAnimMontage* AttackMontage;

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void TryAttack();

	//Sound

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* DetectPlayerSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Movement")
	USoundBase* ChaseSound;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayDetectPlayerSound();

	// 도착 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Movement")
	USoundBase* ArriveAtTargetSound;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayArriveAtTargetSound();

	// 소리 들었을 때 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Perception")
	USoundBase* HearNoiseSound;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayHearNoiseSound();

	//Open Door

	bool HasLineOfSightToTarget(AActor* Target);

	UFUNCTION()
	void CheckDoorAhead();

	// AISense_Hearing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bSoundEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception|Hearing")
	bool bInvestigatingNoise = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Hearing")
	float NoiseInvestigateDuration = 3.0f;

	FTimerHandle NoiseInvestigateTimerHandle;

	UPROPERTY()
	AAIController* CachedAIController = nullptr;

	void StartInvestigateNoise(const FVector& NoiseLocation, AAIController* InController);

	UFUNCTION()
	void EndInvestigateNoise();

	bool IsInvestigatingNoise() const { return bInvestigatingNoise; }

	bool SeePlayerDuringInvestigate();

};