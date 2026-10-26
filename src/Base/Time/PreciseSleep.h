#pragma once
#include <algorithm>
#include <chrono>
#include <concepts>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#pragma comment(lib, "winmm.lib") //MSVC only, GCC ignores this. Base links winmm in its CMakeLists for MinGW.
#endif

#include "Base/Time/Clocks.h"

#include "BasicTypeAliases.h"

#ifdef _WIN32
inline PROCESS_POWER_THROTTLING_STATE PowerThrottling;
#endif



//TODO FOR PROD: find a long term solution to the windows fuckery. This *might* be fine already, idk
//Linux needs no timer resolution fuckery, the default timer slack is ~50us.

namespace KE
{
	template<std::unsigned_integral T>
	void PreciseSleep(T nanos)
	{
#ifdef _WIN32
		timeBeginPeriod(1);
#endif
		Duration current = GlobalClock::QueryGlobalTime();
		Duration end = current + nanos;
		if (nanos > u64(3e6))
			std::this_thread::sleep_for(std::chrono::nanoseconds(nanos - u64(3e6)));

		while (GlobalClock::QueryGlobalTime() < end);
#ifdef _WIN32
		timeEndPeriod(1);
#endif
	}

	template<std::signed_integral T>
	void PreciseSleep(T nanos)
	{
#ifdef _WIN32
		RtlZeroMemory(&PowerThrottling, sizeof(PowerThrottling));
		PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;

		PowerThrottling.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
		PowerThrottling.StateMask = 0;

		SetProcessInformation(GetCurrentProcess(),
			ProcessPowerThrottling,
			&PowerThrottling,
			sizeof(PowerThrottling));

		timeBeginPeriod(1);
#endif
		Duration current = GlobalClock::QueryGlobalTime();
		Duration end = current + Duration(nanos);

		std::this_thread::sleep_for( std::chrono::nanoseconds( std::max( nanos - i64(3e6), i64(0) ) ) );

		while (GlobalClock::QueryGlobalTime() < end);

#ifdef _WIN32
		timeEndPeriod(1);
#endif
	}
}
