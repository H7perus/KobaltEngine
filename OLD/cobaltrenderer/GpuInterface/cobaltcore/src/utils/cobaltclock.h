#pragma once
#include <cstdint>
#include <chrono>
#include <thread>

//to enforce nanoseconds for timekeeping and make it easy. Float time is so 2004

class CobaltClockBase //might be used to keep track of things like simulation time
{
public:
	uint64_t time; //time since program start in nanoseconds
	CobaltClockBase()
	{
		time = 0;
	}
	CobaltClockBase operator+(CobaltClockBase const& obj)
	{
		CobaltClockBase res;
		res.time += obj.time;
		return res;
	}

	float GetFloat()
	{
		return float(time / 1000000000) + float(time % 1000000000) / 1000000000; //seconds + whatever is left over
	}
	virtual void Reset()
	{
		time = 0;
	}
};

class CobaltClock : CobaltClockBase
{
	void MoveClock(int64_t time_passed)
	{
		time += time_passed;
	}
};

class RealtimeCobaltClock : public CobaltClockBase //Note: not actual RT, depends on what the system reports to you.
{
public:
	std::chrono::steady_clock::time_point chrono_curr_time; //whatever the system returned the last time we updated the clock.
	std::chrono::steady_clock::time_point chrono_start_time; //whatever the system returned on last reset or construction.

	RealtimeCobaltClock()
	{
		chrono_start_time = std::chrono::high_resolution_clock::now();
	}

	float Update() 
	{
		chrono_curr_time = std::chrono::high_resolution_clock::now();
		uint64_t prevtime = time;
		time = std::chrono::duration_cast<std::chrono::nanoseconds>(chrono_curr_time - chrono_start_time).count();

		return float(time - prevtime) / 1e9;
	}
	void Reset()
	{
		time = 0;
		chrono_start_time = chrono_curr_time = std::chrono::high_resolution_clock::now();
		chrono_curr_time = chrono_start_time;
	}
};

void SleepFor(int64_t nanoseconds)
{
	if(nanoseconds > 0)
		std::this_thread::sleep_for(std::chrono::nanoseconds(nanoseconds));
}


