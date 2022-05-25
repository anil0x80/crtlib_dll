#include "sse.h"

// msb
#define SIGN_BIT ((1 << 31))
#define FLOAT_NON_FRACTIONAL (8388608.f) /* All single-precision floating point numbers greater than or equal to this have no fractional value. */



namespace sse_math
{
	namespace constants
	{
		__m128 sign_mask  = make_vector(~SIGN_BIT, ~SIGN_BIT, ~SIGN_BIT, ~SIGN_BIT);
		__m128 float_non_fractional = make_vector(FLOAT_NON_FRACTIONAL, FLOAT_NON_FRACTIONAL, FLOAT_NON_FRACTIONAL, FLOAT_NON_FRACTIONAL);
		__m128 float_360  = make_vector(360.f, 360.f, 360.f, 360.f);
		__m128 float_180  = make_vector(180.f, 180.f, 180.f, 180.f);;
		__m128 float_zero = make_vector(0.0f, 0.0f, 0.0f, 0.0f);
	}

}
