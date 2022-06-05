#pragma once

#include "Vector.h"
#include "Ball.h"

#include <array>
#include <vector>

struct TableInfo
{
	static vec_t                          getWidth(), getHeight();
	static const std::vector<Vector2D>&   getTableShape();
	static const std::array<Vector2D, 6>& getPocketsPositions();

	static void setSize(vec_t x, vec_t y);

	struct Position
	{
		static vec_t left();
		static vec_t top();
		static vec_t right();
		static vec_t bottom();
		static void  set(vec_t x, vec_t y, vec_t z, vec_t w);
	};

	static constexpr vec_t getPocketRadius() { return 8.0; /*gameModuleBase + 0x34E90F0*/ }
	static constexpr vec_t getActualWidth()  { return 254; /*gameModuleBase + 0x34E90E8*/ }
	static constexpr vec_t getActualHeight() { return 127; /*gameModuleBase + 0x34E90E0*/ }
	static constexpr vec_t getTableBoundX()  { return getActualWidth()  / 2.0 - Ball::getRadius(); }
	static constexpr vec_t getTableBoundY()  { return getActualHeight() / 2.0 - Ball::getRadius(); }
};

