#include "Balls.h"
#include "Memory.h"
#include "Offsets.h"
#include "globalVars.h"
#include "GameManager.h"

#include <stdio.h>

static SIZE_T ballList;
static SIZE_T ballCount;
static Balls  balls;
Balls*        gBalls = &balls;

bool Balls::init()
{
    SIZE_T buffer;

    buffer = GameManager::getTable();
    buffer = gGlobalVars->memory->read<SIZE_T>(buffer + Offsets::Base);

    ballList  = gGlobalVars->memory->read<SIZE_T>(buffer + Offsets::Entry);
    ballCount = gGlobalVars->memory->read<SIZE_T>(buffer + Offsets::Count);

    return true;
}

Ball* Balls::getBall(SIZE_T index)
{
    return gGlobalVars->memory->read32Pointer<Ball*>(ballList + index * 4UL);
}

SIZE_T Balls::getCount()
{
    return ballCount;
}