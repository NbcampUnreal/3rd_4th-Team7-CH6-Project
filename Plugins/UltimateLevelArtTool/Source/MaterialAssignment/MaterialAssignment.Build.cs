 // Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MaterialAssignment : ModuleRules
{
	public MaterialAssignment(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivatePCHHeaderFile = "Public/MaterialAssignment.h";
        
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Data"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Development"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Interfaces"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Library"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Library"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/UI"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/UI"));
        
        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
        
		/*
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/Engine/Public", //for EditorUtilityWidget
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/Engine/Private", //for EditorUtilityWidget
			}
		);
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/Engine/Public", //for EditorUtilityWidget
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/Engine/Private", //for EditorUtilityWidget
			}
		);*/
					
		PrivateIncludePaths.AddRange(
				new string[] {
					System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutility/Private"
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
			}
			);
		PrivateDependencyModuleNames.AddRange(
            new string[]
            {
	            "Slate",
                "SlateCore",
                "UnrealEd",
                "UMG",
                "UMGEditor",
                "AssetRegistry",
                "PropertyEditor",
                "EditorSubsystem",
                "EditorScriptingUtilities",
                "RHI", 
                "ScriptableEditorWidgets"
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
