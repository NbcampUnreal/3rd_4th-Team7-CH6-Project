// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "MAFunctions.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

FAssetData UMAFunctions::FindTheMostSuitableMaterialForTheNameSlot(const FString& InSlotName, const TArray<FAssetData>& MaterialList)
{
	// Replace underscores and hyphens with spaces
	FString CleanedText = InSlotName.Replace(TEXT("_"), TEXT(" ")).Replace(TEXT("-"), TEXT(" "));

	// Insert spaces before capital letters that follow lowercase letters
	for (int32 Index = 1; Index < CleanedText.Len(); ++Index)
	{
		if (FChar::IsUpper(CleanedText[Index]) && FChar::IsLower(CleanedText[Index - 1]))
		{
			CleanedText = CleanedText.Left(Index) + TEXT(" ") + CleanedText.Mid(Index);
			++Index; // Skip the space we just inserted to avoid considering it in the next iteration
		}
	}

	// Split the cleaned text into words
	TArray<FString> KeyWords;
	CleanedText.ParseIntoArray(KeyWords, TEXT(" "), true);

	if(KeyWords.IsEmpty())
	{
		return {};
	}
	
	const auto KeyNum = KeyWords.Num();

	for (int32 Index = KeyNum - 1; Index >= 0; --Index)
	{
		KeyWords[Index] = KeyWords[Index].ToLower();
		// Ignore words containing "mi" or equal to "m"
		if (KeyWords[Index].Equals(TEXT("mi"),ESearchCase::IgnoreCase) || KeyWords[Index].Equals(TEXT("m"),ESearchCase::IgnoreCase))
		{
			KeyWords.RemoveAt(Index);
		}
	}

	if(KeyWords.IsEmpty())
	{
		return {};
	}
	
	// Find the word with the highest count and its index
	int32 MaxCount = 0;
	int32 MaxIndex = -1;
	
	for(int32 Index = 0; Index < MaterialList.Num() ; Index++)
	{
		int32 AssetWordCount = 0;
		const FString AssetName = MaterialList[Index].AssetName.ToString();
		
		for(auto& KeyWord : KeyWords)
		{
			if(AssetName.Contains(KeyWord,ESearchCase::IgnoreCase))
			{
				++AssetWordCount;
			}
		}

		if(AssetWordCount > MaxCount)
		{
			MaxCount = AssetWordCount;
			MaxIndex = Index;
		}
		if(MaxCount == KeyWords.Num())
		{
			return MaterialList[Index];
		}
	}
	
	if(MaxIndex >= 0)
	{
		return  MaterialList[MaxIndex];
	}
	return {};
}

void UMAFunctions::GetAllMaterialInstances(TArray<FAssetData>& OutAssets)
{
	// Initialize the AssetRegistryModule
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// Set up the filtering conditions with an FARFilter
	FARFilter LocalFilter;
	LocalFilter.ClassPaths.Emplace(UMaterialInstance::StaticClass()->GetClassPathName());
	LocalFilter.PackagePaths.Add(TEXT("/Game"));
	LocalFilter.bRecursivePaths = true; // Include subfolders
	LocalFilter.bRecursiveClasses = true;
	
	// Get the filtered assets from the Asset Registry
	AssetRegistryModule.Get().GetAssets(LocalFilter, OutAssets);
}

bool UMAFunctions::IsMaterialInUsed(UStaticMesh* InStaticMesh, int32 InMaterialIndex)
{
	for (int32 LODIndex = 0; LODIndex < InStaticMesh->GetNumLODs(); ++LODIndex)
	{
		for (int32 SectionIndex = 0; SectionIndex < InStaticMesh->GetNumSections(LODIndex); ++SectionIndex)
		{
			const FMeshSectionInfo Info = InStaticMesh->GetSectionInfoMap().Get(LODIndex, SectionIndex);

			if (Info.MaterialIndex == InMaterialIndex)
			{
				return true;
			}
		}
	}
	
	return false;
}
