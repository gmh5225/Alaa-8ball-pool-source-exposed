#include "AdsManager.h"

#include "Memory.h"
#include "GlobalVars.h"

#include "OneTimeAssignment.h"

static OneTimeAssignment<SIZE_T> adBlockAdress;

void AdsManager::disableAdBreakScreen()
{
	SIZE_T                           adManager, unknownObject;
	UCHAR                            playedMatchesWithoutAd;

	if (adBlockAdress.canAssign()) {
		adManager = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->gameModuleBase + Offsets::SharedAdsManager);
		if (adManager == 0UL)
			return;

		unknownObject = gGlobalVars->memory->read<SIZE_T>(adManager + Offsets::InterstitialsController);
		if (unknownObject == 0UL)
			return;

		adBlockAdress.data = gGlobalVars->memory->read<SIZE_T>(unknownObject + 0xCUL);
		if (adBlockAdress.data)
			adBlockAdress.data += 0x20UL;
	}

	playedMatchesWithoutAd = gGlobalVars->memory->read<UCHAR>(adBlockAdress.data);
	if (playedMatchesWithoutAd > 1)
		gGlobalVars->memory->write(adBlockAdress.data, 1Ui8);
}
