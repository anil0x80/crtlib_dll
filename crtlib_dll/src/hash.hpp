#pragma once
#include <type_traits>

#include "c_string.hpp"
#include "pair.hpp"

namespace crt
{
	// FNV-1a hash implementation
	template <typename T>
	constexpr size_t fnv_1a(const T* ptr, size_t size)
	{
#ifdef _WIN64
		constexpr size_t fnv_prime = 1099511628211u;
		constexpr size_t fnv_offset_basis = 14695981039346656037u;
#else
		constexpr size_t fnv_prime = 16777619u;
		constexpr size_t fnv_offset_basis = 2166136261u;
#endif

		auto hash = fnv_offset_basis;

		for (size_t i = 0; i < size; ++i)
		{
			hash ^= ptr[i];
			hash *= fnv_prime;
		}

		return hash;
	}

	template <typename IntType>
	constexpr std::enable_if_t<std::is_integral_v<IntType>, size_t> get_hash(IntType instance)
	{
		/* the hash of the integer is the integer itself */
		return static_cast<size_t>(instance);
	}

	template <typename EnumType>
	constexpr std::enable_if_t<std::is_enum_v<EnumType>, size_t> get_hash(EnumType instance)
	{
		/* the hash of the integer is the integer itself */
		return static_cast<size_t>(instance);
	}

	// write any specializations needed here

	constexpr size_t get_hash(const char* s)
	{
		return fnv_1a(s, crt::strlen(s));
	}

	template <typename T, typename... Rest>
	constexpr void hash_combine(size_t& seed, const T& v, Rest... rest)
	{
		seed ^= get_hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hash_combine(seed, rest), ...);
	}

	template <typename T, typename K>
	constexpr size_t get_hash(const pair<T, K>& pair)
	{
		size_t seed = 0;
		hash_combine(seed, pair.first(), pair.second());
		return seed;
	}

	constexpr size_t get_hash(float f)
	{
		// -0.0 and 0.0 should return the same hash
		if (f == 0)
			return 0;

		return *(uint32_t*)&f;
	}

	template <typename T>
	constexpr size_t get_hash(const T* pointer)
	{
		const size_t seed = (size_t)pointer;
		return seed ^ (seed >> 16);
	}

}
