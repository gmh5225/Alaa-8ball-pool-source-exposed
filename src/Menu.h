#pragma once

#include <Windows.h>

struct Menu
{
	static bool init(HINSTANCE instance); 
	static void runLoop(), end(HINSTANCE instance);
};

