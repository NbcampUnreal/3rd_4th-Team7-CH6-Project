 // Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AutoSpline : ModuleRules
{
	public AutoSpline(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivatePCHHeaderFile = "Public/AutoSpline.h";
        
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Spline"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Spline"));
        
        PublicIncludePaths.AddRange(
			new string[] {
				
				
			}
        );
					
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"UMG",
			}
		);
		if (Target.Type == TargetRules.TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
				}
			);
		}
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
