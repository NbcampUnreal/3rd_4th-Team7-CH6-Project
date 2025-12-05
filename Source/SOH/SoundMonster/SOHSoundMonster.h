#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHSoundMonster.generated.h"

class ASOHSoundMonsterPatrolRoute;
class ATargetPoint;

UCLASS()
class SOH_API ASOHSoundMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHSoundMonster();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
	ASOHSoundMonsterPatrolRoute* PatrolRouteActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<ATargetPoint*> PatrolTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float LoseSightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float PeripheralVisionAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Hearing")
	float HearingRange;

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void SetMoveSpeed(float NewSpeed);

	bool HasLineOfSightToTarget(AActor* Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackDamage = 20.0f;

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void TryAttack();

};
