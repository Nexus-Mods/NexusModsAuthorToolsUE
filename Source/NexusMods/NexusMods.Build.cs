// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class NexusMods : ModuleRules {
	public NexusMods(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		// ... add public include paths required here ...
		PublicIncludePaths.AddRange(new string[] {
			Path.Combine(ModuleDirectory, "ThirdParty", "miniz"),
		});
		// ... add other private include paths required here ...
		PrivateIncludePaths.AddRange(new string[] {

		});
		// ... add other public dependencies that you statically link with here ...
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
		});
		// ... add private dependencies that you statically link with here ...	
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Projects",
			"InputCore",
			"EditorFramework",
			"UnrealEd",
			"ToolMenus",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"Json",
			"JsonUtilities",
			"HTTP",
			"DesktopPlatform",
			"AssetRegistry",
			"AssetTools",
			"BlueprintGraph",
			"KismetCompiler",
			"EditorScriptingUtilities",
		});
		// ... add any modules that your module loads dynamically here ...
		DynamicallyLoadedModuleNames.AddRange(new string[]{

		});
	}
}
