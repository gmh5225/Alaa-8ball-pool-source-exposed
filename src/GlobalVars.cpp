#include "globalVars.h"
#include "Offsets.h"
#include "Memory.h"

static GlobalVars globalVars;
GlobalVars*       gGlobalVars =&globalVars;

bool GlobalVars::init() 
{
	this->memory                      = new Memory();
	this->gameModuleBase = this->memory->getGameModuleBase();
	if (this->gameModuleBase == 0UL)
		return false;

	sharedGameManager = memory->read<SIZE_T>(this->gameModuleBase + gOffsets::SharedGameManager);
	if (this->sharedGameManager == 0UL)
		return false;

	this->features.adBlock          = false;
	this->features.powerMode        = 1;
	this->features.autoPlayMaxPower = 100.f;
	this->features.randPowerBounds[0] = 50;
	this->features.randPowerBounds[1] = 100;

	this->features.nineBallGameRule  = 0;

	return true;
}