#pragma once

struct Ball;

#include <basetsd.h>

struct Balls
{
	Balls() {}

	bool   init();
	Ball*  getBall(SIZE_T index);
	SIZE_T getCount();

	inline Ball* getCueBall() { return getBall(0UL); }

private:
	struct Offsets
	{
		static constexpr SIZE_T Count = 0x4UL;
		static constexpr SIZE_T Entry = 0xCUL;
		static constexpr SIZE_T Base  = 0x29CUL;
	};
};

extern Balls* gBalls;
