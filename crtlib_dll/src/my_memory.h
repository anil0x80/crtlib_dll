#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "../../crtlib_memory/src/intrinsics_memory.h"

namespace crt
{
	template <typename T>
	T&& move(T& val)
	{
		return reinterpret_cast<T&&>(val);
	}

	template <typename T>
	T&& move(T&& val)
	{
		return reinterpret_cast<T&&>(val);
	}

	template <typename T>
	void swap(T& lhs, T& rhs) noexcept
	{
		T temp = move(lhs);
		lhs = move(rhs);
		rhs = move(temp);
	}
}

