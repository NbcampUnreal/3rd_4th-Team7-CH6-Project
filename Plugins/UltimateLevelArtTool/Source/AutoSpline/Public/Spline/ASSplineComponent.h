#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "ASSplineComponent.generated.h"

DECLARE_DELEGATE(FOnComponentChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AUTOSPLINE_API UASSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	FOnComponentChanged OnComponentChanged;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
