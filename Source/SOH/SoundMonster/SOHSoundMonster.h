#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHSoundMonster.generated.h"

UCLASS()
class SOH_API ASOHSoundMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHSoundMonster();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
