//#pragma once
//#include "cobaltclock.h"
//
//class LoopRateLimiter
//{
//public:
//	RealtimeCobaltClock clock;
//	uint64_t rate_interval;
//	uint64_t last_sleep_time = 0;
//	uint64_t last_true_interval = 0;
//	uint64_t last_time = 0;
//	bool first_time = true;
//	LoopRateLimiter(float times_per_second)
//	{
//		rate_interval = 1000000000 / times_per_second;
//		if (times_per_second == 0)
//			rate_interval = 0;
//	}
//
//	void ChangeRate(float times_per_second)
//	{
//		rate_interval = 1000000000 / times_per_second;
//		if (times_per_second == 0)
//			rate_interval = 0;
//		clock = RealtimeCobaltClock();
//		first_time = true;
//	}
//
//	float GetLastFrametime()
//	{
//		return last_true_interval * 0.000000001;
//	}
//	float GetLastFPS()
//	{
//		return 1000000000 / float(last_true_interval);
//	}
//
//	int64_t Sleep() //if <0, we are in a performance deficit.
//	{
//		if (first_time)
//		{
//			clock.Reset();
//			first_time = false;
//			return 0;
//
//		}
//
//		//determine sleeptime
//
//		clock.Update();
//
//		uint64_t true_interval = clock.time - last_time;
//		int64_t sleep_time = rate_interval - (true_interval - last_sleep_time);
//		last_true_interval = true_interval;
//		last_sleep_time = sleep_time;
//
//		uint64_t target_time = clock.time + sleep_time;
//
//		SleepFor(sleep_time - 4000000); // time - clearance before spinlock
//
//		last_time = clock.time;
//
//		while (clock.time < target_time)
//		{
//			clock.Update();
//		}
//
//		return sleep_time;
//	}
//};