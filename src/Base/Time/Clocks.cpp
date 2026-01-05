#pragma once
#include <cstdint>
#include <chrono>
#include <thread>

#include "Duration.h"
#include "Clocks.h"

#include "BasicTypeAliases.h"

//to enforce nanoseconds for timekeeping and make it easy. Float time is so 2004

namespace KE
{

	// GLOBAL CLOCK

	Duration GlobalClock::QueryGlobalTime()
	{
		u64 time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - launchTime_).count();
		return Duration(time);
	}

	std::chrono::steady_clock::time_point GlobalClock::launchTime_ = std::chrono::high_resolution_clock::now();


	// LOCAL CLOCK

	LocalClock::LocalClock()
	{
		Reset();
	}

	void LocalClock::Reset()
	{
		launchTime_ = std::chrono::high_resolution_clock::now();
	}

	Duration LocalClock::QueryTime()
	{
		return Duration((std::chrono::high_resolution_clock::now() - launchTime_).count());
	}

}