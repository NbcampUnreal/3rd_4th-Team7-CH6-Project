#include "SOHItem.h"
#include "SOHItemManager.h" // 매니저에게 데이터 물어봐야 하니까 필수!
#include "Kismet/GameplayStatics.h" // GameInstance 접근용

ASOHItem::ASOHItem()
{
	// 1. 아이템은 틱이 필요 없음
	PrimaryActorTick.bCanEverTick = false;

	// 2. 외형을 담당할 스태틱 메쉬 컴포넌트 생성
	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    
	// 3. 이것을 루트(몸통)로 설정
	RootComponent = itemMesh;

	// 4. 물리 효과 설정 (바닥에 툭 떨어지게)
	itemMesh->SetSimulatePhysics(true);
	itemMesh->SetCollisionProfileName(TEXT("PhysicsActor")); // 혹은 "BlockAllDynamic"

	// 캐릭터(Pawn)와는 부딪혀서 튕기지 않고, 겹치도록(Overlap) 변경
	// 이렇게 하면 캐릭터가 다가가도 아이템이 날아가지 않음
	itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
	// 카메라(Camera)와도 겹치게 해서, 카메라가 아이템 때문에 줌인/줌아웃 되는 걸 방지
	itemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// E키로 상호작용하기 위해 'Visibility' 채널은 막혀 있어야(Block)함
	// PhysicsActor 프로필은 기본적으로 이게 켜져 있지만, 확실하게 하기 위해 적어줌
	itemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ASOHItem::BeginPlay()
{
	Super::BeginPlay();
    
	// 만약 레벨에 미리 배치해두고 ID를 적어놨다면, 시작하자마자 초기화 진행
	if (!itemID.IsNone())
	{
		InitItem(itemID);
	}
}

void ASOHItem::InitItem(FName newItemID)
{
	// 1. ID 저장
	itemID = newItemID;

	// 2. 게임 인스턴스를 통해 ItemManager 찾기
	UGameInstance* gameInst = GetGameInstance();
	USOHItemManager* itemManager = gameInst ? gameInst->GetSubsystem<USOHItemManager>() : nullptr;

	if (itemManager)
	{
		// 3. 매니저에게 "이 ID에 해당하는 정보 좀 줘" 요청
		FSOHItemTableRow* itemData = itemManager->GetItemDataByID(itemID);

		// 4. 정보가 있고, 메쉬 데이터가 있다면 내 몸(itemMesh)을 갈아입기
		if (itemData && itemData->mesh)
		{
			itemMesh->SetStaticMesh(itemData->mesh);
            
			// (선택사항) 아이템 이름으로 액터 이름 바꾸기 (디버깅용)
			#if WITH_EDITOR
			SetActorLabel(itemData->itemName.ToString());
			#endif
		}
	}
}