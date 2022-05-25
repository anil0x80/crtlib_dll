#include "my_math.hpp"

#include <intrin0.inl.h>

#include "sse.h"

#define SIGN_BIT ((1 << 31))

float crt::fabsf(float val)
{
	union { float f; uint32_t i; } u = { val };
	u.i &= ~SIGN_BIT;
	return u.f;
}

// https://developer.download.nvidia.com/cg/acos.html
float crt::acos(float x)
{
	const float negate = float(x < 0);
	x = fabsf(x);
	float ret = -0.0187293f;
	ret = ret * x;
	ret = ret + 0.0742610f;
	ret = ret * x;
	ret = ret - 0.2121144f;
	ret = ret * x;
	ret = ret + 1.5707288f;
	ret = ret * sse_math::sqrtf(1.0f - x);
	ret = ret - 2 * negate * ret;
	return negate * 3.14159265358979f + ret;
}

float crt::asin(float x)
{
	float negate = float(x < 0);
	x = fabsf(x);
	float ret = -0.0187293f;
	ret *= x;
	ret += 0.0742610f;
	ret *= x;
	ret -= 0.2121144f;
	ret *= x;
	ret += 1.5707288f;
	ret = 3.14159265358979f * 0.5f - sse_math::sqrtf(1.0f - x) * ret;
	return ret - 2 * negate * ret;
}

float crt::sqrt(float x)
{
	return sse_math::sqrtf(x);
}

float crt::to_radian(float degree)
{
	return degree * (3.141592654f / 180.0f);
}


float crt::sin(float x)
{
	//x = crt::to_radian(x);
	 const float a0 = +1.91059300966915117e-31f;
	 const float a1 = +1.00086760103908896f;
	 const float a2 = -1.21276126894734565e-2f;
	 const float a3 = -1.38078780785773762e-1f;
	 const float a4 = -2.67353392911981221e-2f;
	 const float a5 = +2.08026600266304389e-2f;
	 const float a6 = -3.03996055049204407e-3f;
	 const float a7 = +1.38235642404333740e-4f;
	return a0 + x * (a1 + x * (a2 + x * (a3 + x * (a4 + x * (a5 + x * (a6 + x * a7))))));
}

float crt::cos(float x)
{
	//x = crt::to_radian(x);
	// New implementation. Also generated using lolremez.
	// Old version significantly deviated from expected results.
	 const float a0 = 9.9995999154986614e-1f;
	 const float a1 = 1.2548995793001028e-3f;
	 const float a2 = -5.0648546280678015e-1f;
	 const float a3 = 1.2942246466519995e-2f;
	 const float a4 = 2.8668384702547972e-2f;
	 const float a5 = 7.3726485210586547e-3f;
	 const float a6 = -3.8510875386947414e-3f;
	 const float a7 = 4.7196604604366623e-4f;
	 const float a8 = -1.8776444013090451e-5f;
	return a0 + x * (a1 + x * (a2 + x * (a3 + x * (a4 + x * (a5 + x * (a6 + x * (a7 + x * a8)))))));
}




int crt::floor(float x)
{
	if (x >= 0)
	{
		return (int)x;
	}
	else
	{
		int y = (int)x;
		return ((float)y == x) ? y : y - 1;
	}
}

int crt::ceil(float x)
{
	int inum = (int)x;
	if (x == (float)inum) {
		return inum;
	}
	return inum + 1;
}

unsigned long crt::next_power_of_2(unsigned long val)
{
	unsigned long index;
	_BitScanReverse(&index, --val);
	if (val == 0)
		index = (unsigned long)-1;
	return (1U << (index + 1));
}

float crt::atan2(float Y, float X)
{
	const float absX = fabsf(X);
	const float absY = fabsf(Y);
	const bool yAbsBigger = (absY > absX);
	float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
	float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

	if (t0 == 0.f)
		return 0.f;

	float t3 = t1 / t0;
	float t4 = t3 * t3;

	static const float c[7] = {
		+7.2128853633444123e-03f,
		-3.5059680836411644e-02f,
		+8.1675882859940430e-02f,
		-1.3374657325451267e-01f,
		+1.9856563505717162e-01f,
		-3.3324998579202170e-01f,
		+1.0f
	};

	t0 = c[0];
	t0 = t0 * t4 + c[1];
	t0 = t0 * t4 + c[2];
	t0 = t0 * t4 + c[3];
	t0 = t0 * t4 + c[4];
	t0 = t0 * t4 + c[5];
	t0 = t0 * t4 + c[6];
	t3 = t0 * t3;

	t3 = yAbsBigger ? (0.5f * PI) - t3 : t3;
	t3 = (X < 0.0f) ? PI - t3 : t3;
	t3 = (Y < 0.0f) ? -t3 : t3;

	return t3;
}

float crt::atan(float x)
{
	return atan2(x, 1.f);
}