#pragma once

#ifdef _WIN32
	#ifdef RENDERER_DLL_EXPORTS
		#define RENDERER_DLL_API __declspec(dllexport)
	#else
		#define RENDERER_DLL_API __declspec(dllimport)
	#endif
#else
		#define RENDERER_DLL_API // Empty for non-Windows platforms
#endif