// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class UltimateLevelArtTool : ModuleRules
{
	public UltimateLevelArtTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivatePCHHeaderFile = "Public/UltimateLevelArtTool.h";

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Development"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Style"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Style"));

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
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
				"Blutility" //Editor Utility Widget
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "ToolMenus",
                "LevelEditor",
                "UnrealEd",			 //GEditor
                "Projects",			//IPluginManager
                "UMGEditor"
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
