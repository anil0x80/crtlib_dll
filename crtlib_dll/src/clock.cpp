#include "clock.h"

static double g_milliseconds_frequency;
static double g_microseconds_frequency;
static bool g_initialized = false;

crt::clock crt::clock::now()
{
	if (!g_initialized) [[unlikely]]
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);

		g_milliseconds_frequency = static_cast<double>(li.QuadPart) / 1000.0;
		g_microseconds_frequency = static_cast<double>(li.QuadPart) / 1000000.0;
		g_initialized = true;
	}

	LARGE_INTEGER current_counter;
	QueryPerformanceCounter(&current_counter);

	clock result;
	result.start_time = current_counter;

	return result;
}

double crt::clock::difference_ms(const clock& other) const
{
	return static_cast<double>(start_time.QuadPart - other.start_time.QuadPart) / g_milliseconds_frequency;
}

double crt::clock::difference_us(const clock& other) const
{
	return static_cast<double>(start_time.QuadPart - other.start_time.QuadPart) / g_microseconds_frequency;
}

