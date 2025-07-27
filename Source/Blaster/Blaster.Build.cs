// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Blaster : ModuleRules
{
	public Blaster(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 这里列出的任何模块都可以从我们项目中访问
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"OnlineSubsystem",
			"OnlineSubsystemSteam"
			});
	}
}
