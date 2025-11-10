#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "SOHEQSContext_LastKnownLocation.generated.h"

UCLASS()
class SOH_API USOHEQSContext_LastKnownLocation : public UEnvQueryContext
{
	GENERATED_BODY()
	
protected:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
		FEnvQueryContextData& ContextData) const override;
};
