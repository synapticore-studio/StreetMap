using UnrealBuildTool;

public class StreetMapRuntime : ModuleRules
{
	public StreetMapRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"PCG"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"RHI",
				"RenderCore",
				"NavigationSystem"
			}
		);
	}
}