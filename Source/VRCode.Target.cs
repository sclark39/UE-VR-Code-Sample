/*
* Author: Skyler Clark (@sclark39)
* Website: http://skylerclark.com
* License: MIT License
*/

using UnrealBuildTool;
using System.Collections.Generic;

public class VRCodeTarget : TargetRules
{
	public VRCodeTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
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
