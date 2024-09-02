// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EOSItchIntegration : ModuleRules
{
	public EOSItchIntegration(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "HttpServer" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
