#pragma once
#include "Common.h"
///@file BuiltinScripts.h  Ready-made behaviours shipped with the engine.
///
/// Scripts (all parameters are floats editable in the inspector):
///  - Rotator                : spins the entity.            Speed (deg/s)
///  - Oscillator             : moves back and forth.        Amplitude, Frequency, Horizontal (0/1)
///  - PlatformerController2D : keyboard controlled body.    Speed, JumpForce, RespawnY
///  - CameraFollow2D         : follows the player entity.   Smoothing, OffsetX, OffsetY

namespace Matrix::scripting
{
    /// Registers every built-in script with the ScriptRegistry. Called by the engine during start-up.
    MATRIX_DLL void RegisterBuiltinScripts();
}
