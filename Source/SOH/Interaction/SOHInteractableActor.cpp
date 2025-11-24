#include "Interaction/SOHInteractableActor.h"
#include "Components/MeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInterface.h"

ASOHInteractableActor::ASOHInteractableActor()
{
	PrimaryActorTick.bCanEverTick = true; InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidget->SetDrawSize(FVector2D(200.f, 100.f));
	InteractionWidget->SetVisibility(false);
}
void ASOHInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	if (InteractionWidgetClass)
	{
		InteractionWidget->SetWidgetClass(InteractionWidgetClass);
	}
}
void ASOHInteractableActor::Interact_Implementation(AActor* Caller)
{
	// C++ 기본 정의: Interact가 호출되었음을 알리는 로그 출력 if (Caller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Base Interact called on %s by %s. No specific action defined in C++."), *GetName(), *Caller->GetName());
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
	 if (InteractionWidget)
	 {
		 InteractionWidget->SetVisibility(true);
	 }
}
void ASOHInteractableActor::HideInteractWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
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
			//Mesh->GetStaticMesh()->NaniteSettings.bEnabled = false;
		}
	}
}