// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EOSTutorial : ModuleRules
{
	public EOSTutorial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" , "OnlineSubsystemEOS", "OnlineSubsystem","OnlineSubsystemUtils" });
	}
}
