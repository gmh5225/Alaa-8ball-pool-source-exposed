#pragma once

struct Ball;

#include "Vector.h"

#include <basetsd.h>

struct GenericData
{
	GenericData()  = delete;
	~GenericData() = delete;

	vec_t getAimAngle();
	void setAimAngle(const vec_t& angle);
	Ball* getAimAngleTarget();
	vec_t distanceToAimTarget();

	static bool get(GenericData** out);

private:
	struct Offsets
	{
		struct Unknown
		{
			static constexpr SIZE_T GuidlineFlags       = 0x14UL; // 1  = Don't Show / 257 = Show
			static constexpr SIZE_T AimAngle            = 0x18UL;
			static constexpr SIZE_T Timer               = 0x28UL;
			static constexpr SIZE_T AimAngleTarget      = 0xCUL;
			static constexpr SIZE_T AimAngleTargetState = 0x20UL;
			static constexpr SIZE_T DistanceToAimTarget = 0x30UL;
			static constexpr SIZE_T Base                = 0x27CUL;
		};

		static constexpr SIZE_T Base = 0x2D8UL;
	};
};

