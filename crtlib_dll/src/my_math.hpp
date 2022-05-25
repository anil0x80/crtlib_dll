#pragma once
#include <cstdint>

#define PI (3.1415926535897932f)
#define FLOAT_EQUAL(f1, f2, epsilon) (crt::fabsf((f1) - (f2)) <= (epsilon))

namespace crt
{
	constexpr auto rad_to_deg_constant = 180.f / PI;

	/* fast O(lg(power)) exponentiation algorithm. */
	template <typename T>
	T exponent(T base, size_t power)
	{
		//result * base^power is an invariant quantity.
		for (T result = 1;;)
		{
			if (power == 0)
				return result;
			if (power % 2 == 0)
			{
				base *= base;
				power /= 2;
			}
			else
			{
				power -= 1;
				result *= base;
			}
		}
		
	}

	/** Clamps X to be between Min and Max, inclusive */
	template< class T >
	[[nodiscard]] static __forceinline T clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}


	float atan2(float Y, float X);
	float atan(float x);
	float fabsf(float val);
	float acos(float x);
	float sin(float x);
	float cos(float x);
	float asin(float x);
	float sqrt(float x);
	float cos(float x);
	float to_radian(float degree);

	int floor(float x);
	int ceil(float x);

	template <typename T>
	T abs(T val)
	{
		return (val < (T)0) ? (-val) : val;
	}

	template <typename T>
	T max_(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template <typename T>
	T min_(T a, T b)
	{
		return (a < b) ? a : b;
	}

	unsigned long next_power_of_2(unsigned long val);

}
