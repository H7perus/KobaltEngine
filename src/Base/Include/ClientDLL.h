#pragma once

#ifdef _WIN32
#ifdef CLIENT_DLL_EXPORTS
#define CLIENT_DLL_API __declspec(dllexport)
#else
#define CLIENT_DLL_API __declspec(dllimport)
#endif
#else
#define CLIENT_DLL_API // Empty for non-Windows platforms
#endif