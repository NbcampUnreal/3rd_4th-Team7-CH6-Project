#include "SOHSpiderMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

void USOHSpiderMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwnerPawn = TryGetPawnOwner();
	if (!OwnerPawn)
	{
		Speed = 0.f;
		return;
	}

	FVector Velocity = OwnerPawn->GetVelocity();
	Velocity.Z = 0.f;

	Speed = Velocity.Size();
}