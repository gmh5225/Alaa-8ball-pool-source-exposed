#include "Ball.h"
#include "Memory.h"
#include "Offsets.h"
#include "GlobalVars.h"

Vector3D Ball::getSpin()
{
    return gGlobalVars->memory->read<Vector3D>(reinterpret_cast<SIZE_T>(this) + Offsets::Spin);
}

Vector2D Ball::getPosition()
{
    return gGlobalVars->memory->read<Vector2D>(reinterpret_cast<SIZE_T>(this) + Offsets::Position);
}

Vector2D Ball::getVelocity()
{
    return gGlobalVars->memory->read<Vector2D>(reinterpret_cast<SIZE_T>(this) + Offsets::Velocity);
}

int Ball::classification()
{
    return gGlobalVars->memory->read<int>(reinterpret_cast<SIZE_T>(this) + Offsets::classification);
}

bool Ball::isOnTable()
{
    int state = gGlobalVars->memory->read<int>(reinterpret_cast<SIZE_T>(this) + Offsets::State);

    return (bool)((state == 1) || (state == 2));
}
