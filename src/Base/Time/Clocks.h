#pragma once
#include <cstdint>

#include <chrono>

#include <thread>


#include "Duration.h"

#include "BasicTypeAliases.h"


#include "BaseDLL.h"
//to enforce nanoseconds for timekeeping and make it easy. Float time is so 2004

using namespace std::chrono_literals;

namespace KE
{
	class BASE_DLL_API GlobalClock
	{
	public:
		static Duration QueryGlobalTime();
	private:
		static std::chrono::steady_clock::time_point launchTime_;
	};

	//This is intended for local timekeeping. Unlike GlobalClock, it is...local
	class LocalClock
	{
	public:
		LocalClock();
		void Reset();
		Duration QueryTime();

	private:
		std::chrono::steady_clock::time_point launchTime_;
	};

}




//class BaseClock //might be used to keep track of things like simulation time
//{
//public:
//	u64 time; //time since program start in nanoseconds
//	CobaltClockBase()
//	{
//		time = 0;
//	}
//	CobaltClockBase operator+(CobaltClockBase const& obj)
//	{
//		CobaltClockBase res;
//		res.time += obj.time;
//		return res;
//	}
//
//	f32 GetFloat()
//	{
//		return f32(time / 1000000000) + f32(time % 1000000000) / 1000000000; //seconds + whatever is left over
//	}
//	virtual void Reset()
//	{
//		time = 0;
//	}
//};
//
//class CobaltClock : CobaltClockBase
//{
//	void MoveClock(i64 time_passed)
//	{
//		time += time_passed;
//	}
//};
//
//class RealtimeCobaltClock : public CobaltClockBase //Note: not actual RT, depends on what the system reports to you.
//{
//public:
//	std::chrono::steady_clock::time_point chrono_curr_time; //whatever the system returned the last time we updated the clock.
//	std::chrono::steady_clock::time_point chrono_start_time; //whatever the system returned on last reset or construction.
//
//	RealtimeCobaltClock()
//	{
//		chrono_start_time = std::chrono::high_resolution_clock::now();
//	}
//
//	float Update() 
//	{
//		chrono_curr_time = std::chrono::high_resolution_clock::now();
//		u64 prevtime = time;
//		time = std::chrono::duration_cast<std::chrono::nanoseconds>(chrono_curr_time - chrono_start_time).count();
//
//		return f32(time - prevtime) / 1e9;
//	}
//	void Reset()
//	{
//		time = 0;
//		chrono_start_time = chrono_curr_time = std::chrono::high_resolution_clock::now();
//		chrono_curr_time = chrono_start_time;
//	}
//};
//
//void SleepFor(i64 nanoseconds)
//{
//	if(nanoseconds > 0)
//		std::this_thread::sleep_for(std::chrono::nanoseconds(nanoseconds));
//}


