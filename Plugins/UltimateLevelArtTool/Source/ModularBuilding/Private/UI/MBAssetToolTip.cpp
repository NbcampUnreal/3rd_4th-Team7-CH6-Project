// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBAssetToolTip.h"
#include "AssetRegistry/AssetData.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/StaticMesh.h"
#include "PhysicsEngine/BodySetup.h"

void UMBAssetToolTip::SetTooltipData(const FAssetData& InAssetData) const
{
	AssetNameText->SetText(FText::FromName(InAssetData.AssetName));

	FString TooltipContent;
	
	//Find Path
	FString Path = InAssetData.GetSoftObjectPath().GetAssetPathString();
	int32 DotIndex = INDEX_NONE;
	Path.FindLastChar('.', DotIndex);
	if (DotIndex != INDEX_NONE)
	{
		Path = 	Path.LeftChop(Path.Len() - DotIndex);
	}
	TooltipContent.Append(FString::Printf(TEXT("<Title>Path:</> %s\n"),*Path));
	
	//Find Materials
	if(const auto StaticMesh = Cast<UStaticMesh>(InAssetData.GetAsset()))
	{
		TooltipContent.Append(FString::Printf(TEXT("<Title>Materials:</> %d\n"),StaticMesh->GetStaticMaterials().Num()));

		TooltipContent.Append(FString::Printf(TEXT("<Title>Vertices:</> %d\n"),StaticMesh->GetNumVertices(0)));
		TooltipContent.Append(FString::Printf(TEXT("<Title>Triangles:</> %d\n"),StaticMesh->GetNumTriangles(0)));

		const FVector Extent = StaticMesh->GetExtendedBounds().BoxExtent * 2.0f;
		TooltipContent.Append(FString::Printf(TEXT("<Title>Approx Size:</> %d x %d x %d\n"),FMath::RoundToInt(Extent.X),FMath::RoundToInt(Extent.Y),FMath::RoundToInt(Extent.Z)));
		
		TooltipContent.Append(FString::Printf(TEXT("<Title>Nanite Enabled:</> %s\n"),StaticMesh->NaniteSettings.bEnabled ? TEXT("True") : TEXT("False")));

		TooltipContent.Append(FString::Printf(TEXT("<Title>LODs:</> %d\n"),StaticMesh->GetNumLODs()));
		
		TooltipContent.Append(FString::Printf(TEXT("<Title>Num Collision Primitives:</> %d\n"),StaticMesh->GetBodySetup()->AggGeom.GetElementCount()));

		FString CollisionComplexityName;
		UEnum::GetValueAsString(StaticMesh->GetBodySetup()->CollisionTraceFlag, CollisionComplexityName);
		TooltipContent.Append(FString::Printf(TEXT("<Title>Collision Complexity:</> %s\n"),*CollisionComplexityName));
		
		TooltipContent.Append(FString::Printf(TEXT("\nDouble-click to open the asset in the SM Editor.")));
	}
	AssetInfoText->SetText(FText::FromString(TooltipContent));
}
