using UnrealBuildTool;
using System.Collections.Generic;

public class TP_VirtualRealityTarget : TargetRules
{
	public TP_VirtualRealityTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        ExtraModuleNames.AddRange(new string[] { "TP_VirtualReality" });
    }
}
