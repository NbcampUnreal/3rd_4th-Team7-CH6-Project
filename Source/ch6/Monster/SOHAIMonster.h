#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHAIMonster.generated.h"

UCLASS()
class CH6_API ASOHAIMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHAIMonster();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
