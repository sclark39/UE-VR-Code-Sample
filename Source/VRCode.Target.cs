/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

using UnrealBuildTool;
using System.Collections.Generic;

public class VRCodeTarget : TargetRules
{
	public VRCodeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        ExtraModuleNames.AddRange(new string[] { "VRCode" });
    }
}
