/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

using UnrealBuildTool;
using System.Collections.Generic;

public class VRCodeEditorTarget : TargetRules
{
    public VRCodeEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new string[] { "VRCode" });
    }
}
