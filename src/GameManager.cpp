#include "GameManager.h"

#include "OneTimeAssignment.h"
#include "GlobalVars.h"
#include "Memory.h"
#include "Offsets.h"
#include <stdio.h>

static OneTimeAssignment<SIZE_T> is9BallGameAddress;
static OneTimeAssignment<SIZE_T> classificationsAddress;
static OneTimeAssignment<SIZE_T> stateAddress;

bool GameManager::is9BallGame()
{
	int result;

	if (is9BallGameAddress.canAssign()) {
		is9BallGameAddress.data = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + Offsets::Rules);
	}

	result = gGlobalVars->memory->read<int>(is9BallGameAddress.data);

	return (result & 64) == 64;
}

bool GameManager::isPlayerTurn()
{
	return (GameManager::getState() == 4);
}

int GameManager::getGameMode()
{
	return gGlobalVars->memory->read<int>(gGlobalVars->sharedGameManager + Offsets::GameMode);
}

int GameManager::getState()
{
	SIZE_T stateManager, objectEntry, lastObject, objectCount, buffer;

	if (stateAddress.canAssign()) {
		stateManager = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + Offsets::StateManager);
		buffer       = gGlobalVars->memory->read<SIZE_T>(stateManager + 0x4UL);
		objectCount  = gGlobalVars->memory->read<SIZE_T>(buffer + 0x4UL);
		objectEntry  = gGlobalVars->memory->read<SIZE_T>(buffer + 0xCUL);
		lastObject   = gGlobalVars->memory->read<SIZE_T>(objectEntry + objectCount * 0x4UL - 0x4UL);
		stateAddress.data = lastObject;
		if (stateAddress.data)
			stateAddress.data += 0xCUL;
	}

	return gGlobalVars->memory->read<int>(stateAddress.data);
}

int GameManager::playerClassification()
{
	SIZE_T rules; 

	if (classificationsAddress.canAssign()) {
		rules                       = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + Offsets::Rules);
		classificationsAddress.data = gGlobalVars->memory->read<SIZE_T>(rules + 0x4CUL);
	}

	return gGlobalVars->memory->read<int>(classificationsAddress.data + 0UL); // 0 * 4 for local player 1 * 4 for oppenent 
}

SIZE_T GameManager::getTable()
{
	return gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + Offsets::Table);
}
