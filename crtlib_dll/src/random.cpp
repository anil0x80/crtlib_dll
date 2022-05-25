#include "random.h"
#include "mersenne_twister.h"
#include "crt_windows.h"

static bool seeded = false;

static void seed_if_needed()
{
	if(!seeded) [[unlikely]]
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);

		seed(ft.dwLowDateTime);
		seeded = true;
	}
}

uint32_t crt::r_uint32()
{
	seed_if_needed();
	return rand_u32();
}

uint8_t crt::r_uint8()
{
	return static_cast<uint8_t>(r_uint32());
}

crt::string crt::r_string(size_t size)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	crt::string tmp_s;
	tmp_s.reserve(size);

	for (size_t i = 0; i < size; ++i) {
		tmp_s.push_back(alphanum[r_uint32() % (sizeof(alphanum) - 1)]);
	}

	return tmp_s;
}

crt::string crt::r_string(size_t min_size, size_t max_size)
{
	return r_string(r_uint32() % (max_size - min_size + 1) + min_size);
}

float crt::r_float(float low, float high)
{
	return low + static_cast <float> (r_uint32()) / (static_cast <float> (UINT32_MAX / (high - low)));
}