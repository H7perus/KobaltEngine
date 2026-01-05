#pragma once

#ifdef _WIN32
#ifdef BASE_DLL_EXPORTS
#define BASE_DLL_API __declspec(dllexport)
#else
#define BASE_DLL_API __declspec(dllimport)
#endif
#else
#define BASE_DLL_API // Empty for non-Windows platforms
#endif