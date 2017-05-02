# UE VR Code Sample

This is a WIP conversion of UE's VR Blueprint template into a C++ project.

## Implemented
- Pawn creates camera at appropriate height for device
- Hands are tracked
- Hand grip animation

## Mostly Implemented
- Pickup items
- Teleport
- Controller rumble

## Todo
- Roomscale / Chaperone stuff
- Improve inefficiency of deleting/recreating spline
- HMD Locomotion Pawn

I've started with converting the MotionControllerPawn and MotionControllerMap.

In commit messages you may see the prefix [nf]. This is to mark commits that are either in a 'not finished' or 'not functional' state.

If you see things that are not to Unreal Engine's coding standard, or know an easier way to accomplish anything please let me know or do a pull request!
