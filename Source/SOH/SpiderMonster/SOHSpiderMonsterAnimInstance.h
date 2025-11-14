#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SOHSpiderMonsterAnimInstance.generated.h"

UCLASS()
class SOH_API USOHSpiderMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed;
};