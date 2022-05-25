#pragma once
#include <cstdint>

#include "my_string.hpp"


namespace crt
{
	/* returns a random 32 bit unsigned integer */
	uint32_t r_uint32();

	/* returns a random 8 bit unsigned integer */
	uint8_t r_uint8();

	/* returns a random string of length size */
	crt::string r_string(size_t size);

	/* returns a random string with length € [min_size, max_size]*/
	crt::string r_string(size_t min_size, size_t max_size);

	/* returns a random float between [low, high]*/
	float r_float(float low, float high);
}
