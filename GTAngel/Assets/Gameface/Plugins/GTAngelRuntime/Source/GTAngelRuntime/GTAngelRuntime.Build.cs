using UnrealBuildTool;

public class GTAngelRuntime : ModuleRules
{
    public GTAngelRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Json",
            "JsonUtilities",
            "Projects"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add("GTANGEL_WITH_NAMED_PIPES=1");
        }
        else
        {
            PublicDefinitions.Add("GTANGEL_WITH_NAMED_PIPES=0");
        }
    }
}
