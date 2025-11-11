 // Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AutoSplineEditor : ModuleRules
{
	public AutoSplineEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivatePCHHeaderFile = "Public/AutoSplineEditor.h";
        
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Editor"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Editor"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/UI"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/UI"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Development"));
        
        PublicIncludePaths.AddRange(
			new string[] {
				
			}
        );
					
		PrivateIncludePaths.AddRange(
			new string[] {
				"AutoSpline/Public/Spline",
				"AutoSpline/Private/Spline"
			}
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Blutility",
				"DetailCustomizations",
				"EditorStyle",
				"AutoSpline"
				
				
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"UMG",
				"UnrealEd",
				//"EditorWidgets",
				"UMGEditor",
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
