#pragma once

#include <basetsd.h>

struct GameManager
{
	static bool   is9BallGame();
	static bool   isPlayerTurn();
	static int    getGameMode();
	static int    getState(), playerClassification();
	static SIZE_T getTable();

private:
	struct Offsets
	{
		static constexpr SIZE_T Rules        = 0x2A8UL;
		static constexpr SIZE_T StateManager = 0x300UL;
		static constexpr SIZE_T Table        = 0x2ACUL;
		static constexpr SIZE_T GameMode     = 0x370UL;
	};
};

