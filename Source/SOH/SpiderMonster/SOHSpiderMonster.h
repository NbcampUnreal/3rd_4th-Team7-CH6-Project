#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHSpiderMonster.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
struct FAIStimulus;
class ASOHAIMonster;

UCLASS()
class SOH_API ASOHSpiderMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHSpiderMonster();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void OnPlayerSpotted(AActor* PlayerActor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI")
	ASOHAIMonster* MainMonsterRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* ScreamSound;

};
