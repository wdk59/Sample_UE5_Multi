// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Sample_Multi : ModuleRules
{
	public Sample_Multi(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
            "Niagara"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Sample_Multi",
			"Sample_Multi/Variant_Platforming",
			"Sample_Multi/Variant_Platforming/Animation",
			"Sample_Multi/Variant_Combat",
			"Sample_Multi/Variant_Combat/AI",
			"Sample_Multi/Variant_Combat/Animation",
			"Sample_Multi/Variant_Combat/Gameplay",
			"Sample_Multi/Variant_Combat/Interfaces",
			"Sample_Multi/Variant_Combat/UI",
			"Sample_Multi/Variant_SideScrolling",
			"Sample_Multi/Variant_SideScrolling/AI",
			"Sample_Multi/Variant_SideScrolling/Gameplay",
			"Sample_Multi/Variant_SideScrolling/Interfaces",
			"Sample_Multi/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
