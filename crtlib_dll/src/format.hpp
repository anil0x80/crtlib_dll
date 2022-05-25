#pragma once
#include "my_string.hpp"
#include "to_string.h"

namespace crt
{
	namespace detail
	{
		inline crt::string do_format_replace(char* start, char* end)
		{
			return crt::string{ start };
		}

		template <typename T, typename... Types>
		crt::string do_format_replace(char* start, char* end, T token, Types... tokens)
		{
			const auto token_start = crt::find_first_of(start, end, '{');
			CRT_ASSERT(token_start != end, "invalid format");

			const auto token_end = crt::find_first_of(token_start + 1, end, '}');
			CRT_ASSERT(token_end != end, "invalid format");

			crt::string final_string{};
			const auto token_string = crt::to_string(token);

			const auto left_substr_size = token_start - start;
			if (left_substr_size > 0)
			{
				crt::string left_substr{ start, (size_t)left_substr_size };
				left_substr += token_string;
				final_string = move(left_substr);
			}
			else
			{
				final_string = move(token_string);
			}

			const auto right_substr_size = end - (token_end + 1);
			if (right_substr_size > 0)
			{
				final_string += token_end + 1;
			}

			return detail::do_format_replace(&*final_string.begin(), &*final_string.end(), tokens...);
		}
	}

	// format("My name is {}, hello from {}", "Jane", "USA") == "My name is Jane, hello from USA"
	template <typename T, typename... Types>
	crt::string format(const char* fmt, T token, Types... tokens)
	{
		crt::string buffer{ fmt };
		return detail::do_format_replace(&*buffer.begin(), &*buffer.end(), crt::move(token), std::forward<Types>(tokens)...);
	}

	inline crt::string format(const char* fmt)
	{
		return crt::string{ fmt };
	}


	inline crt::string format(const crt::string& fmt)
	{
		return fmt;
	}

}