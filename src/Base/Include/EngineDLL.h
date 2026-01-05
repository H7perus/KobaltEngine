#pragma once

#ifdef _WIN32
#ifdef ENGINE_DLL_EXPORTS
#define ENGINE_DLL_API __declspec(dllexport)
#else
#define ENGINE_DLL_API __declspec(dllimport)
#endif
#else
#define ENGINE_DLL_API // Empty for non-Windows platforms
#endif