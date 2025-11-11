 // Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ModularBuilding : ModuleRules
{
	public ModularBuilding(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivatePCHHeaderFile = "Public/ModularBuilding.h";
        
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Building"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Building"));        
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Development"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Interfaces"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Libraries"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Libraries"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Slate"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Slate"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/UI"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/UI"));
        
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
				"Blutility"
				
			}
			);
		PrivateDependencyModuleNames.AddRange(
            new string[]
            {
	            "Slate",
                "SlateCore",
                "UnrealEd",
                "EditorSubsystem",
                "UMG",
                "UMGEditor",
                "LevelEditor",
                "EditorScriptingUtilities",
                "ImageWrapper",
                "AssetRegistry",
                "EditorWidgets", //Drop Target
                "PhysicsCore", 
                "ScriptableEditorWidgets",
            }
            );
        
        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
				"ContentBrowser"
			}
			);
	}
}
