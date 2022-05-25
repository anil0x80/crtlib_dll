#pragma once
#include <cstdint>
#include <emmintrin.h>
#include <xmmintrin.h>
#define FORCEINLINE __forceinline



namespace sse_math
{
	namespace constants
	{
		extern __m128 sign_mask;
		extern __m128 float_non_fractional;
		extern __m128 float_360;
		extern __m128 float_180;
		extern __m128 float_zero;
	}


	[[nodiscard]] FORCEINLINE __m128 make_vector(int32_t v1, int32_t v2, int32_t v3, int32_t v4)
	{
		union { __m128 v; __m128i i; } temp;
		temp.i = _mm_setr_epi32(v1, v2, v3, v4);
		return temp.v;
	}

	[[nodiscard]] FORCEINLINE __m128 make_vector(float v1, float v2, float v3, float v4)
	{
		return _mm_setr_ps(v1, v2, v3, v4);
	}


	[[nodiscard]] FORCEINLINE __m128 load_float(float f)
	{
		return _mm_load_ss(&f);
	}

	[[nodiscard]] FORCEINLINE float sqrtf(float val)
	{
		return _mm_sqrt_ss(load_float(val)).m128_f32[0];
	}

	[[nodiscard]] FORCEINLINE float fabsf(float val)
	{
		return _mm_and_ps(load_float(val), constants::sign_mask).m128_f32[0];
	}

	[[nodiscard]] FORCEINLINE __m128 vector_divide(const __m128& x, const __m128& y)
	{
		return _mm_div_ps(x, y);
	}

	[[nodiscard]] FORCEINLINE __m128 vector_add(const __m128& x, const __m128& y)
	{
		return _mm_add_ps(x, y);
	}

	[[nodiscard]] FORCEINLINE __m128 vector_subtract(const __m128& x, const __m128& y)
	{
		return _mm_sub_ps(x, y);
	}

	[[nodiscard]] FORCEINLINE __m128 vector_compare_eq(const __m128& x, const __m128& y)
	{
		return _mm_cmpeq_ps(x, y);
	}

	[[nodiscard]] FORCEINLINE __m128 vector_compare_ge(const __m128& x, const __m128& y)
	{
		return _mm_cmpge_ps(x, y); // dst[i + 31:i] : = (a[i + 31:i] >= b[i + 31:i]) ? 0xFFFFFFFF : 0
	}

	[[nodiscard]] FORCEINLINE __m128 vector_compare_gt(const __m128& x, const __m128& y)
	{
		return _mm_cmpgt_ps(x, y); // dst[i + 31:i] : = (a[i + 31:i] >= b[i + 31:i]) ? 0xFFFFFFFF : 0
	}

	[[nodiscard]] FORCEINLINE __m128 vector_abs(const __m128& x)
	{
		return _mm_and_ps(x, constants::sign_mask);
	}

	// if mask is 1, use the bit from vec1, otherwise vec2
	[[nodiscard]] FORCEINLINE __m128 vector_select(const __m128& mask, const __m128& vec1, const __m128& vec2)
	{
		return _mm_xor_ps(vec2, _mm_and_ps(mask, _mm_xor_ps(vec1, vec2)));
	}

	[[nodiscard]] FORCEINLINE __m128 vector_truncate(const __m128& X)
	{
		return _mm_cvtepi32_ps(_mm_cvttps_epi32(X)); // convert to int, then convert back to float.
	}

	// returns vec3 - vec1 * vec2
	[[nodiscard]] FORCEINLINE __m128 vector_negate_multiply_add(const __m128& vec1, const __m128& vec2, const __m128& vec3)
	{
		return _mm_sub_ps(vec3, _mm_mul_ps(vec1, vec2));
	}

	// memberwise max
	[[nodiscard]] FORCEINLINE __m128 vector_max(const __m128& vec1, const __m128& vec2)
	{
		return _mm_max_ps(vec1, vec2); //dst[i+31:i] := MAX(a[i+31:i], b[i+31:i])
	}

	[[nodiscard]] FORCEINLINE __m128 vector_min(const __m128& vec1, const __m128& vec2)
	{
		return _mm_min_ps(vec1, vec2); //dst[i+31:i] := MIN(a[i+31:i], b[i+31:i])
	}

	// returns 0 - vec
	[[nodiscard]] FORCEINLINE __m128 vector_negate(const __m128& vec)
	{
		return _mm_sub_ps(_mm_setzero_ps(), vec);
	}

	[[nodiscard]] FORCEINLINE __m128 vector_zero()
	{
		return _mm_setzero_ps();
	}

	[[nodiscard]] FORCEINLINE bool is_equal(const __m128& vec1, const __m128& vec2)
	{
		const auto result = vector_compare_eq(vec1, vec2);
		return _mm_movemask_ps(result) == 0xf;
	}


	//#define VectorNegate( Vec )				_mm_sub_ps(_mm_setzero_ps(),Vec)

	[[nodiscard]] FORCEINLINE __m128 vector_mod(const __m128& x, const __m128& y)
	{
		const __m128 division = vector_divide(x, y);
		// Floats where abs(f) >= 2^23 have no fractional portion, and larger values would overflow VectorTruncate.
		const __m128 no_fraction_mask = vector_compare_ge(vector_abs(division), constants::float_non_fractional);
		const __m128 temp = vector_select(no_fraction_mask, division, vector_truncate(division));
		const __m128 result = vector_negate_multiply_add(y, temp, x);
		// Clamp to [-AbsY, AbsY] because of possible failures for very large numbers (>1e10) due to precision loss.
		const __m128 abs_y = vector_abs(y);
		return vector_max(vector_negate(abs_y), vector_min(result, abs_y));
	}

}
