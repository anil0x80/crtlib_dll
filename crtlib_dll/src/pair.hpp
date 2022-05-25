#pragma once

#include "my_memory.h"

namespace crt
{
	template <typename T, typename K>
	class pair
	{
	public:
		constexpr pair() = default;

		constexpr pair(T first, K second) : first_(crt::move(first)), second_(crt::move(second))
		{

		}

		constexpr T& first()
		{
			return first_;
		}

		constexpr K& second()
		{
			return second_;

		}

		constexpr const T& first() const
		{
			return first_;
		}

		constexpr const K& second() const
		{
			return second_;

		}

		constexpr friend bool operator==(const pair& l, const pair& r)
		{
			return l.first() == r.first() && l.second() == r.second();
		}

	private:
		T first_{};
		K second_{};
	};

	template <typename T, typename K>
	constexpr static pair<T, K> make_pair(T first, K second)
	{
		return pair(crt::move(first), crt::move(second));
	}
}