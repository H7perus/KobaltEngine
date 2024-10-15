#pragma once

#include <iostream>
#include "SDL2/SDL.h"
#undef main
#include "GL/glew.h"
#include "glm/glm.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#ifdef _WIN32
	#ifdef MYDLL_EXPORTS
		#define MYDLL_API __declspec(dllexport)
	#else
		#define MYDLL_API __declspec(dllimport)
	#endif
#else
	#define MYDLL_API // Empty for non-Windows platforms
#endif

MYDLL_API void testfunc();