/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

using UnrealBuildTool;
using System.Collections.Generic;

public class VRCodeEditorTarget : TargetRules
{
	public VRCodeEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "VRCode" } );
	}
}
