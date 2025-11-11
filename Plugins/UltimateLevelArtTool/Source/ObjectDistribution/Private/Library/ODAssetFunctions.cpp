// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODAssetFunctions.h"
#include "FileHelpers.h"
#include "IImageWrapperModule.h"
#include "ObjectTools.h"
#include "IImageWrapper.h"
#include "ImageUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture2D.h"

FAssetData UODAssetFunctions::GetAssetDataFromObject(const UObject* Object)
{
	const FString assetPath = GetAssetPathFromObject(Object);
	return GetAssetDataFromPath(assetPath);
}

FString UODAssetFunctions::GetAssetPathFromObject(const UObject* AssetObject)
{
	FString path;
	if (AssetObject == nullptr) return path;

	AssetObject->GetPathName().Split("/", &path, nullptr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	return FString::Printf(TEXT("%s/%s.%s"), *path, *AssetObject->GetName(), *AssetObject->GetName());
}

FAssetData UODAssetFunctions::GetAssetDataFromPath(const FString& AssetPath)
{
	FAssetData AssetData = FAssetData();
	if (AssetPath.IsEmpty())
	{
		return AssetData;
	}

	if (FEditorFileUtils::IsMapPackageAsset(AssetPath)) {
		return AssetData;
	}

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(AssetPath);
	return AssetData;
}

UTexture2D* UODAssetFunctions::GenerateThumbnailForSM(const FAssetData& InAssetPath)
{
	FString PackageFilename;
	const FName ObjectFullName = FName(*InAssetPath.GetFullName());
	TSet<FName> ObjectFullNames;
	ObjectFullNames.Add(ObjectFullName);

	if (FPackageName::DoesPackageExist(InAssetPath.PackageName.ToString(), &PackageFilename))
	{
		FThumbnailMap ThumbnailMap;
		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

		ThumbnailTools::LoadThumbnailsFromPackage(PackageFilename, ObjectFullNames,ThumbnailMap);

		if(const FObjectThumbnail* AssetTN = ThumbnailMap.Find(ObjectFullName))
		{
			const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
			if(ImageWrapper.Get())
			{
				ImageWrapper->SetRaw(AssetTN->GetUncompressedImageData().GetData(), AssetTN->GetUncompressedImageData().Num(),
				AssetTN->GetImageWidth(), AssetTN->GetImageHeight(), ERGBFormat::BGRA, 8);
				const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();

				if(UTexture2D* CreatedTexture = FImageUtils::ImportBufferAsTexture2D(CompressedByteArray))
				{
					return CreatedTexture;
				}
			}
		}
		else
		{
			if(const auto GeneratedObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(InAssetPath.GetAsset()))
			{
				const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
				if(ImageWrapper.Get())
				{
					ImageWrapper->SetRaw(GeneratedObjectThumbnail->GetUncompressedImageData().GetData(), GeneratedObjectThumbnail->GetUncompressedImageData().Num(),
					GeneratedObjectThumbnail->GetImageWidth(), GeneratedObjectThumbnail->GetImageHeight(), ERGBFormat::BGRA, 8);
					const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();

					if(UTexture2D* CreatedTexture = FImageUtils::ImportBufferAsTexture2D(CompressedByteArray))
					{
						return  CreatedTexture;
					}
				}	
			}
		}
	}
	const FSoftObjectPath PlaceholderTexturePath("/Engine/EditorResources/S_ReflActorIcon");
	if(UObject* ToolWindowObject = PlaceholderTexturePath.TryLoad())
	{
		if(const auto PlaceHolderTexture = Cast<UTexture2D>(ToolWindowObject))
		{
			return PlaceHolderTexture;
		}
	}
	return nullptr;
}

