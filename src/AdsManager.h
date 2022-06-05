#pragma once

#include <basetsd.h>

struct AdsManager
{
	static void disableAdBreakScreen();
private:
	struct Offsets
	{
		static constexpr SIZE_T SharedAdsManager        = 0x3526B0CUL;
		static constexpr SIZE_T InterstitialsController = 0x84UL;
	};
};