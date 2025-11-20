#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SOHSoundMonsterBTService.generated.h"

UCLASS()
class SOH_API USOHSoundMonsterBTService : public UBTService
{
	GENERATED_BODY()

public:
	USOHSoundMonsterBTService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};