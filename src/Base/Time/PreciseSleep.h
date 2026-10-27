#pragma once
#include <algorithm>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#pragma comment(lib, "winmm.lib") //MSVC only, GCC ignores this. Base links winmm in its CMakeLists for MinGW.
#endif

#include "Base/Time/Clocks.h"

#include "BasicTypeAliases.h"

//Coarse OS sleep until shortly before the target, then spin on the GlobalClock for the rest.
//Windows: high resolution waitable timer (Win10 1803+), wakes within a fraction of a ms and doesn't care about timeBeginPeriod.
//  Do NOT use std::this_thread::sleep_for there, on MinGW it goes through winpthreads and overshoots by several ms.
//Linux needs no timer resolution fuckery, the default timer slack is ~50us.

#ifdef _WIN32
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#endif

namespace KE
{
	//how long before the target we stop sleeping and start spinning
#ifdef _WIN32
	inline constexpr i64 spinMarginHighResTimer = i64(1e6); //0.5ms is too low to be entirely reliable
	inline constexpr i64 spinMarginLowResTimer  = i64(3e6);
#else
	inline constexpr i64 spinMargin = i64(1e6);
#endif

#ifdef _WIN32
	namespace detail
	{
		//only needed for the low resolution fallback: force 1ms timer resolution once and stop windows from ignoring it
		inline void EnableLowResTimerFallback()
		{
			static bool done = []
			{
				PROCESS_POWER_THROTTLING_STATE powerThrottling{};
				powerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
				powerThrottling.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
				powerThrottling.StateMask = 0;

				SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &powerThrottling, sizeof(powerThrottling));

				timeBeginPeriod(1);
				return true;
			}();
			(void)done;
		}

		struct SleepTimer
		{
			HANDLE handle = nullptr;
			i64 spinMargin = spinMarginHighResTimer;

			SleepTimer()
			{
				handle = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);

				if (!handle)
				{
					handle = CreateWaitableTimerW(nullptr, TRUE, nullptr);
					spinMargin = spinMarginLowResTimer;
					EnableLowResTimerFallback();
				}
			}

			~SleepTimer()
			{
				if (handle)
					CloseHandle(handle);
			}

			void SleepFor(i64 nanos)
			{
				if (!handle)
				{
					::Sleep(DWORD(nanos / 1'000'000));
					return;
				}

				LARGE_INTEGER due;
				due.QuadPart = -(nanos / 100); //negative = relative, in 100ns units
				if (SetWaitableTimer(handle, &due, 0, nullptr, nullptr, FALSE))
					WaitForSingleObject(handle, INFINITE);
			}
		};

		inline SleepTimer& GetSleepTimer()
		{
			thread_local SleepTimer timer;
			return timer;
		}
	}
#endif

	template<std::signed_integral T>
	void PreciseSleep(T nanos)
	{
		Duration current = GlobalClock::QueryGlobalTime();
		Duration end = current + Duration(nanos);

#ifdef _WIN32
		detail::SleepTimer& timer = detail::GetSleepTimer();
		i64 coarse = i64(nanos) - timer.spinMargin;
		if (coarse > 0)
			timer.SleepFor(coarse);
#else
		std::this_thread::sleep_for( std::chrono::nanoseconds( std::max( i64(nanos) - spinMargin, i64(0) ) ) );
#endif

		while (GlobalClock::QueryGlobalTime() < end);
	}

	template<std::unsigned_integral T>
	void PreciseSleep(T nanos)
	{
		PreciseSleep(i64(std::min<u64>(nanos, u64(INT64_MAX))));
	}
}
