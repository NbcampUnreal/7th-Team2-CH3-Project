// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Exodus : ModuleRules
{
	public Exodus(ReadOnlyTargetRules Target) : base(Target)
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
			"Exodus",
			"Exodus/Variant_Platforming",
			"Exodus/Variant_Platforming/Animation",
			"Exodus/Variant_Combat",
			"Exodus/Variant_Combat/AI",
			"Exodus/Variant_Combat/Animation",
			"Exodus/Variant_Combat/Gameplay",
			"Exodus/Variant_Combat/Interfaces",
			"Exodus/Variant_Combat/UI",
			"Exodus/Variant_SideScrolling",
			"Exodus/Variant_SideScrolling/AI",
			"Exodus/Variant_SideScrolling/Gameplay",
			"Exodus/Variant_SideScrolling/Interfaces",
			"Exodus/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
