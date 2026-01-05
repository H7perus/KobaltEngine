#pragma once
#include <concepts>
#include <thread>

#define NOMINMAX
#include <windows.h>
#pragma comment(lib, "winmm.lib")

#include "Base/Time/Clocks.h"

#include "BasicTypeAliases.h"

PROCESS_POWER_THROTTLING_STATE PowerThrottling;



//TODO FOR PROD: find a long term solution to the windows fuckery. This *might* be fine already, idk

namespace KE
{
	template<std::unsigned_integral T>
	void PreciseSleep(T nanos)
	{
		timeBeginPeriod(1);
		Duration current = GlobalClock::QueryGlobalTime();
		Duration end = current + nanos;
		std::this_thread::sleep_for(nanos - u64(3e6));

		while (GlobalClock::QueryGlobalTime() < end);
		timeEndPeriod(1);
	}

	template<std::signed_integral T>
	void PreciseSleep(T nanos)
	{
		RtlZeroMemory(&PowerThrottling, sizeof(PowerThrottling));
		PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;

		PowerThrottling.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
		PowerThrottling.StateMask = 0;

		SetProcessInformation(GetCurrentProcess(),
			ProcessPowerThrottling,
			&PowerThrottling,
			sizeof(PowerThrottling));

		timeBeginPeriod(1);
		Duration current = GlobalClock::QueryGlobalTime();
		Duration end = current + nanos;

		std::this_thread::sleep_for( std::chrono::nanoseconds( std::max( nanos - i64(3e6), i64(0) ) ) );

		while (GlobalClock::QueryGlobalTime() < end);

		timeEndPeriod(1);
	}
}