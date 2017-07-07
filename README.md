# Unreal VR Template C++ Sample

Switch to the `templatizing` branch and copy the project folder into your templates directory to use this as a template for your project!

I was disappointed that there was no C++ version of Unreal 4.16's VR template, so I decided to make one.

I have an Oculus Rift, so for the Rift I made the teleport work more like Robo Rally since the original control scheme was pretty awkward. This can be changed in the Pawn's settings.

If you see things that are not to Unreal Engine's coding standard, or know an easier way to accomplish anything please let me know or do a pull request!

I want this to be a learning resource that anyone can use, so I am publishing it under the MIT License but if this license causes an issue for you please let me know!

## Implemented
- HMD Locomotion Pawn
- Teleport with Stick Input
- PSVR Controller Roll to Teleport
- Pickup and throw physics based objects
- Create Teleport splines and do traces in C++
- Create default implementations of all components in C++, to be filled in from Editor
  - Added a workaround for issue UE-41708 in the Blueprint for VRPawn_BP; setting the ChildActorComponent to another code blueprint causes issues in UE4.14 and UE4.15

## Changed
- Teleport changed to be activated/deactivated by stick input like Robo Rally, rather than a seperate button for Rift (optional)
- Added a workaround (via a blueprint) to trigger haptic feedback when the rumble event occurs since Touch has no rumble capability

## Missing
- Rumble not implemented
- SteamVRChaperone / RoomScaleMesh not implemented

See Issues tab for info on the progress of these missing features. I do not have a Vive or PSVR, so they are untested. I am happy to accept pull requests for any issues found.
