
#include "Interaction/SOHInteractableActor.h"
#include "Components/MeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

ASOHInteractableActor::ASOHInteractableActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASOHInteractableActor::Interact_Implementation(AActor* Caller)
{
	// C++ 기본 정의: Interact가 호출되었음을 알리는 로그 출력
	if (Caller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Base Interact called on %s by %s. No specific action defined in C++."), 
			*GetName(), *Caller->GetName());
	}
}

bool ASOHInteractableActor::CanReceiveTrace_Implementation(AActor* Caller, bool bCanInteract)
{
	if (bCanInteract)
	{
		ShowInteractWidget();
		ApplyOverlayMaterial(OutlineMaterial);
		return true;
	}
	else
	{
		HideInteractWidget();
		ApplyOverlayMaterial(nullptr);
		return false;
	}
}

void ASOHInteractableActor::ShowInteractWidget()
{
	if (InteractionWidgetClass && !InteractionWidgetInstance)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			InteractionWidgetInstance = CreateWidget<UUserWidget>(PC, InteractionWidgetClass);
			if (InteractionWidgetInstance)
			{
				InteractionWidgetInstance->AddToViewport();
			}
		}
	}
}
void ASOHInteractableActor::HideInteractWidget()
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->RemoveFromParent();
		InteractionWidgetInstance = nullptr;
	}
}
void ASOHInteractableActor::ApplyOverlayMaterial(UMaterialInterface* Material)
{
	TArray<UMeshComponent*> MeshComponents;
	GetComponents<UMeshComponent>(MeshComponents);

	for (UMeshComponent* Mesh : MeshComponents)
	{
		if (Mesh)
		{
			Mesh->SetOverlayMaterial(Material);
		}
	}
}