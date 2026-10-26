#pragma once

#include <cstdint>

//Durations can be timestamps, but also be used as intervals and such
struct KDuration
{
	uint64_t timeNS;

	operator float() 
	{
		return static_cast<float>(static_cast<double>(timeNS) / 1e9);
	}

	float fsecs()
	{
		return static_cast<float>(static_cast<double>(timeNS) / 1e9);
	}
	float fmsecs()
	{
		return static_cast<float>(static_cast<double>(timeNS) / 1e6);
	}
};