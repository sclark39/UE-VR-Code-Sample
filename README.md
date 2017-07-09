# Unreal VR Template C++ Sample

### Setup to use as a sample project
- Clone 'master' branch
- Open uproject file

### Setup to use as a Template
- Switch to 'templatizing' branch
- Open up explorer to C:\Program Files\Epic Games\UE_4.16\Templates
- Copy TP_VirtualReality, TP_VirtualRealityBP, and TemplateResources folders in
- Launch Unreal, create a project using the template, and enjoy.

### About

I was disappointed that there was no C++ version of Unreal 4.14's VR template, so I decided to make one.

I have an Oculus Rift, so for the Rift I made the teleport work more like Robo Rally since the original control scheme was pretty awkward. This can be changed in the Pawn's settings.

If you see things that are not to Unreal Engine's coding standard, or know an easier way to accomplish anything please let me know or do a pull request!

I want this to be a learning resource that anyone can use, so I am publishing it under the MIT License but if this license causes an issue for you please let me know!

## Changed
- Teleport changed to be activated/deactivated by stick input like Robo Rally, rather than a seperate button for Rift (optional)
- Added a workaround (via a blueprint) to trigger haptic feedback when the rumble event occurs since Touch has no rumble capability
- Fixed an issue in the HMDLocomotionPawn Blueprint which was passing in an incorrect launch velocity

## Missing
- SteamVRChaperone / RoomScaleMesh not implemented

See Issues tab for info on the progress of these missing features. I do not have a Vive or PSVR, so they are untested. I am happy to accept pull requests for any issues found.
