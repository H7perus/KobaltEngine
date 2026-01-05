#pragma once

#include <algorithm>

#include "Clocks.h"
#include "PreciseSleep.h"

#include "BasicTypeAliases.h"

namespace KE
{
	class LoopRateLimiter
	{
	public:

		LoopRateLimiter(f32 times_per_second)
		{
			interval_ = u64(1e9 / times_per_second);
			if (times_per_second == 0)
				interval_ = 0;
		}

		void SetInterval(f32 interval)
		{
			interval_ = u64(interval * 1e9f);
		}

		void SetRate(f32 times_per_second)
		{
			interval_ = u64(1e9 / times_per_second);
			if (times_per_second == 0)
				interval_ = 0;
		}

		f32 GetLastInterval()
		{
			return last_true_interval_;
		}

		f32 GetLastRate()
		{
			return 1.0f / f32(last_true_interval_);
		}

		i64 Sleep() //if <0, we are in a performance deficit.
		{

			KE::Duration current_time = KE::GlobalClock::QueryGlobalTime();
			KE::Duration true_interval = current_time - last_time_;


			i64 sleep_duration = i64(interval_) - i64(true_interval - last_sleep_duration_);

			PreciseSleep(sleep_duration);

			last_sleep_duration_ = KE::Duration(sleep_duration);
			last_time_ = current_time;
			last_true_interval_ = true_interval;

			return sleep_duration;
		}

	private:
		KE::Duration interval_;
		KE::Duration last_sleep_duration_;
		KE::Duration last_true_interval_;
		KE::Duration last_time_;
	};
}
