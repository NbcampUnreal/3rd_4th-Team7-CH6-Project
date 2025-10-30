#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SOHAIMonsterAnimInstance.generated.h"

UCLASS()
class SOH_API USOHAIMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float Speed;
	
};
