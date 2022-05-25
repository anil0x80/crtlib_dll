#pragma once
#include <cstdint>
#include "crt_windows.h"

namespace crt
{
	class clock
	{
	public:
		static clock now();

		// this - other
		double difference_ms(const clock& other) const;
		double difference_us(const clock& other) const;

	private:
		LARGE_INTEGER start_time{};
	};
}
