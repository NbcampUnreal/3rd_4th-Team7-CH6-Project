#include "SOHLockActor.h"
#include "SOHInventoryComponent.h" // 인벤토리 확인
#include "Level/SOHSlidingDoor.h"
#include "Level/SOHOpenDoor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

ASOHLockActor::ASOHLockActor()
{
	// 메쉬 생성
	LockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockMesh"));
	RootComponent = LockMesh;
    
	// 자물쇠는 물리 적용 X (고정된 물체)
	LockMesh->SetSimulatePhysics(false);
    
	// 상호작용을 위해 Visibility 채널 Block
	LockMesh->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
}

void ASOHLockActor::Interact_Implementation(AActor* Caller)
{
	Super::Interact_Implementation(Caller);

	if (!Caller) return;

	// 1. 말을 건 사람(Caller)의 인벤토리를 가져옵니다.
	USOHInventoryComponent* InventoryComp = Caller->FindComponentByClass<USOHInventoryComponent>();

	if (InventoryComp)
	{
		// 2. 필요한 열쇠가 설정되어 있는지 확인
		if (RequiredKeyID.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Lock] 이 자물쇠는 잠겨있지 않습니다. (KeyID 설정 안됨)"));
			for (ASOHSlidingDoor* Door : TargetSlidingDoor)
			{
				if (Door)
				{
					Door->UnlockSlidingDoor(Caller);
				}
			}

			// 일반 문 해제
			for (ASOHOpenDoor* Door : TargetOpenDoor)
			{
				if (Door)
				{
					Door->UnlockOpenDoor(Caller); // 해당 함수가 ASOHOpenDoor에 정의되어 있어야 함
				}
			}
			Destroy(); // 자물쇠 삭제
			return;
			
		}

		// 3. 인벤토리에 해당 열쇠가 몇 개 있는지 검사
		int32 KeyCount = InventoryComp->GetItemQuantity(RequiredKeyID);

		if (KeyCount > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[Lock] 성공! 열쇠(%s)를 사용하여 잠금을 해제했습니다."), *RequiredKeyID.ToString());

			// 연결된 문에게 잠금 해제 명령
			for (ASOHSlidingDoor* Door : TargetSlidingDoor)
			{
				if (Door)
				{
					Door->UnlockSlidingDoor(Caller);
				}
			}

			// 일반 문 해제
			for (ASOHOpenDoor* Door : TargetOpenDoor)
			{
				if (Door)
				{
					Door->UnlockOpenDoor(Caller); // 해당 함수가 ASOHOpenDoor에 정의되어 있어야 함
				}
			}

			// 잠금해제 사운드
			if (UnlockSound)
			{
				UGameplayStatics::SpawnSoundAtLocation(
					this,
					UnlockSound,
					GetActorLocation()
				);
			}

			// 4. 잠금 해제 연출
			Destroy(); // 자물쇠 삭제
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Lock] 실패! 열쇠(%s)가 없습니다."), *RequiredKeyID.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lock] 인벤토리가 없는 대상이 접근했습니다."));
	}
}