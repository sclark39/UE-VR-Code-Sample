# Unreal VR Template C++ Sample

I was disappointed that there was no C++ version of Unreal 4.14's VR template, so I decided to make one.

I have an Oculus Rift, so I made the teleport work more like Robo Rally. There are a bunch of Steam VR things that I did not implement because I do not have a Vive. I'd be happy to accept merge requests.

If you see things that are not to Unreal Engine's coding standard, or know an easier way to accomplish anything please let me know or do a pull request!

I want this to be a learning resource that anyone can use, so I am publishing it under the MIT License but if this license causes an issue for you please let me know!

## Implemented in C++
- Teleport with Stick Input
- Pickup and throw physics based objects
- Create Teleport splines and do traces in C++
- Create default implementations of all components in C++, to be filled in from Editor
⋅⋅- Added a workaround for issue UE-41708 in the Blueprint for VRPawn_BP, setting the ChildActorComponent to another code blueprint causes issues in UE4.14 and UE4.15

## Changed Features
- Teleport changed to be activated/deactivated by stick input like Robo Rally, rather than a seperate button
- Added a workaround (via a blueprint) to trigger haptic feedback when the rumble event occurs since Touch has no rumble capability

## Missing Features
- PSVR Controller Roll to Teleport
- HMD Locomotion Pawn
- SteamVRChaperone not implemented
- RoomScaleMesh not implememented
- Rumble not implemented

