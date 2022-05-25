#pragma once
#include <type_traits>

namespace crt
{
	struct random_iterator_tag {};
	struct forward_iterator_tag {};

	template<typename It>
	auto advance(It it, size_t by)
	{
		if constexpr (std::is_same_v<decltype(It::tag()), forward_iterator_tag>)
		{
			for (size_t i = 0; i < by; ++i)
				++it;
			return it;
		}
		else
		{
			return it + by;
		}
	}

	template<typename It>
	auto distance(It first, It last)
	{
		if constexpr (std::is_same_v<decltype(It::tag()), forward_iterator_tag>)
		{
			size_t result = 0;

			while (first != last)
			{
				++result;
				++first;
			}

			return result;
		}
		else
		{
			return last - first;
		}
	}
}