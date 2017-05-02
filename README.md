# UE VR Code Sample

This is a WIP conversion of UE's VR Blueprint template into a C++ project.

## Implemented
- Pawn creates camera at appropriate height for device
- Hands are tracked
- Hand grip animation
- Teleport
- Pickup items

## Mostly Implemented
- Controller rumble (Oculus Touch only has Haptics, not Rumble)

## Todo
- Roomscale / Chaperone stuff (I don't have a Vive nor PSVR)
- Improve inefficiency of deleting/recreating spline
- HMD Locomotion Pawn 

I've started with converting the MotionControllerPawn and MotionControllerMap.

In commit messages you may see the prefix [nf]. This is to mark commits that are either in a 'not finished' or 'not functional' state.

If you see things that are not to Unreal Engine's coding standard, or know an easier way to accomplish anything please let me know or do a pull request!
