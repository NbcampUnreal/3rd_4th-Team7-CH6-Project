// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ch6 : ModuleRules
{
	public ch6(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ch6",
			"ch6/Variant_Platforming",
			"ch6/Variant_Platforming/Animation",
			"ch6/Variant_Combat",
			"ch6/Variant_Combat/AI",
			"ch6/Variant_Combat/Animation",
			"ch6/Variant_Combat/Gameplay",
			"ch6/Variant_Combat/Interfaces",
			"ch6/Variant_Combat/UI",
			"ch6/Variant_SideScrolling",
			"ch6/Variant_SideScrolling/AI",
			"ch6/Variant_SideScrolling/Gameplay",
			"ch6/Variant_SideScrolling/Interfaces",
			"ch6/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
