#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SOHSoundMonsterBTDecorator.generated.h"

UCLASS()
class SOH_API USOHSoundMonsterBTDecorator : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USOHSoundMonsterBTDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
