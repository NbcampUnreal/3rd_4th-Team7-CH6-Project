#include "SOHEQSContext_LastKnownLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

static const FName KEY_LastKnown(TEXT("LastKnownLocation"));

void USOHEQSContext_LastKnownLocation::ProvideContext(
    FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    AActor* QuerierActor = Cast<AActor>(QueryInstance.Owner.Get());
    FVector Out = QuerierActor ? QuerierActor->GetActorLocation() : FVector::ZeroVector;

    UBlackboardComponent* BB = nullptr;
    if (AAIController* AIC = Cast<AAIController>(QuerierActor ? QuerierActor->GetInstigatorController()
        : nullptr))
    {
        BB = AIC->GetBlackboardComponent();
    }
    else if (AAIController* AIC2 = Cast<AAIController>(QueryInstance.Owner.Get()))
    {
        BB = AIC2->GetBlackboardComponent();
    }

    if (BB && BB->IsVectorValueSet(KEY_LastKnown))
    {
        FVector LK = BB->GetValueAsVector(KEY_LastKnown);

        if (UWorld* W = QuerierActor ? QuerierActor->GetWorld() : nullptr)
        {
            if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(W))
            {
                FNavLocation Proj;
                if (Nav->ProjectPointToNavigation(LK, Proj, FVector(300, 300, 300)))
                {
                    Out = Proj.Location;
                }
                else
                {
                    Out = LK;
                }
            }
            else
            {
                Out = LK;
            }
        }
    }
    UEnvQueryItemType_Point::SetContextHelper(ContextData, Out);
}
