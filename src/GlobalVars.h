#pragma once

#include "Vector.h"

#include <basetsd.h>

struct Memory;

struct GlobalVars
{
	inline GlobalVars() { ZeroMemory(this, sizeof(*this)); }

	Memory* memory;
	SIZE_T  gameModuleBase;
	SIZE_T  sharedGameManager;

	struct
	{
		Vector2D scale;

	} gameloopWindowInfo;

	struct
	{
		struct
		{
			bool predicionPath;
			bool drawShotState;
			bool simpleGuideLine;
		} esp;

		int   powerMode;
		int   nineBallGameRule;
		float autoPlayMaxPower;
		int   randPowerBounds[2];
		bool  prediction;
		int   automatic;

		bool adBlock;

	} features;

	bool init();
};

extern GlobalVars* gGlobalVars;