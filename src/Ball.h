#pragma once

#include "Vector.h"

#include <basetsd.h>

struct Ball
{
	Ball()  = delete;
    ~Ball() = delete;

	Vector3D getSpin();
	Vector2D getPosition();
	Vector2D getVelocity();

	int  classification();
	bool isOnTable();

	static constexpr vec_t getRadius() 
	{ 
		return 3.800475; /* this + Offsets::SharedGameManager::Table::Balls::Ball::Radius */
	}

private:
	struct Offsets
	{
		static constexpr SIZE_T Spin           = 0x8UL;
		static constexpr SIZE_T Position       = 0x20UL;
		static constexpr SIZE_T Velocity       = 0x30UL;
		static constexpr SIZE_T Radius         = 0x50UL;
		static constexpr SIZE_T classification = 0x7CUL;
		static constexpr SIZE_T State          = 0x80UL; // 1 or 2 = On Table
	};
};

