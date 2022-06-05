#include "GenericData.h"
#include "GlobalVars.h"
#include "Memory.h"

#include "Offsets.h"

vec_t GenericData::getAimAngle()
{
    return gGlobalVars->memory->read<vec_t>(reinterpret_cast<SIZE_T>(this) + Offsets::Unknown::AimAngle);
}

void GenericData::setAimAngle(const vec_t& angle)
{
    gGlobalVars->memory->write(reinterpret_cast<SIZE_T>(this) + Offsets::Unknown::AimAngle, angle);
}

Ball* GenericData::getAimAngleTarget()
{
    return gGlobalVars->memory->read<Ball*>(reinterpret_cast<SIZE_T>(this) + Offsets::Unknown::AimAngleTarget);
}

vec_t GenericData::distanceToAimTarget()
{
    return gGlobalVars->memory->read<vec_t>(reinterpret_cast<SIZE_T>(this) + Offsets::Unknown::DistanceToAimTarget);
}

bool GenericData::get(GenericData** out)
{
    SIZE_T genericData = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + Offsets::Base);

    *out = gGlobalVars->memory->read<GenericData*>(genericData + Offsets::Unknown::Base);
    if (*out == nullptr)
        return false;

    return true;
}
