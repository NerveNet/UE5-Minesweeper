// Copyright 2022 Brad Monahan. All Rights Reserved.

using UnrealBuildTool;
using System.IO;


public class MinesweeperEditor : ModuleRules
{
	public MinesweeperEditor(ReadOnlyTargetRules Target) : base(Target)
    {
	    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	    //PrivatePCHHeaderFile = "Private/MinesweeperPCH.h";
	    bEnforceIWYU = true;
	    
	    
	    PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Public")
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Private")
			}
		);
		
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"MinesweeperRuntime",
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "SlateCore", 
                "Slate",
				"Projects",
				"UnrealEd",
				"EditorStyle",
                "LevelEditor",
				"DeveloperSettings",
				"Settings"
			}
        );

    }
}
