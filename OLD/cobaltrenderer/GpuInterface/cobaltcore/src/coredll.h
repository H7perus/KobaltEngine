#pragma once

#ifdef _WIN32
	#ifdef CORE_DLL_EXPORTS
		#define CORE_DLL_API __declspec(dllexport)
	#else
		#define CORE_DLL_API __declspec(dllimport)
	#endif
#else
		#define CORE_DLL_API // Empty for non-Windows platforms
#endif