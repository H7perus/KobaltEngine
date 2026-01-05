#pragma once

#include <cstdint>
#include <cassert>

#include "BasicTypeAliases.h"

//Durations can be timestamps, but also be used as intervals and such
namespace KE
{
	struct Duration
	{
		u64 nanos = 0;

		Duration() {};

		template<std::signed_integral T>
		Duration(T durr)
		{
			nanos = u64(std::max(durr, T(0)));
		}

		template<std::unsigned_integral T>
		Duration(T durr)
		{
			nanos = u64(durr);
		}

		Duration operator+(const Duration& other) const {
			return Duration{ nanos + other.nanos };
		}
		Duration operator-(const Duration& other) const {
			assert(nanos - other.nanos <= nanos);
			return Duration{ nanos - other.nanos };
		}

		auto operator<=>(const Duration& other) const {
			return nanos <=> other.nanos;
		}

		template<std::integral T>
		operator T() const {
			return static_cast<T>(nanos);
		}

		template<std::floating_point T>
		operator T()
		{
			return static_cast<T>(static_cast<f64>(nanos) / 1e9);
		}

		f32 fmsecs() const
		{
			return static_cast<f32>(static_cast<f64>(nanos) / 1e6);
		}
	};
}