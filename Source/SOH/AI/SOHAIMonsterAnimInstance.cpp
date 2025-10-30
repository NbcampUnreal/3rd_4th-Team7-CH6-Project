#include "SOHAIMonsterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

void USOHAIMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwnerPawn = TryGetPawnOwner();
	if (!OwnerPawn) return;

	FVector Velocity = OwnerPawn->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

}
