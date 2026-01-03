using UnrealBuildTool;

public class StreetMapImporting : ModuleRules
{
	public StreetMapImporting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"StreetMapRuntime"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"XmlParser",
				"AssetTools",
				"Projects",
				"Slate",
				"SlateCore",
				"PropertyEditor",
				"RenderCore",
				"RHI",
				"RawMesh",
				"AssetRegistry"
			}
		);
	}
}