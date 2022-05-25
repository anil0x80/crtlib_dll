#pragma once
/**
 * @file stack_string.hpp
 *
 * @brief C++20 compile time string encryption library, supports both char and wchar_t literals.
 *
 * @author radulf
 *
 */
#include <type_traits>

#define DECRYPT_STRING(str) crt::util::stack_string<str, crt::util::details::get_compile_time_seconds() + (size_t)__COUNTER__ * 2>().decrypt()

namespace crt::util::details
{
	constexpr char get_compile_time_seconds()
	{
		auto str_seconds = __TIME__ + 6;
		return static_cast<char>(*str_seconds - '0') * 10 +
			static_cast<char>(str_seconds[1] - '0');
	}
}

namespace crt::util
{
	template<typename T, size_t N>
	struct string_literal
	{
		constexpr string_literal(const T(&str)[N])
		{
			for (size_t i = 0; i < N; ++i)
				value[i] = str[i];
		}

		T value[N];
	};

	template <string_literal StringLiteral, size_t K, size_t N = _countof(StringLiteral.value)>
	class stack_string
	{
	public:
		using char_type = std::remove_all_extents_t<decltype(StringLiteral.value)>;

		consteval stack_string()
		{
			for (size_t i = 0; i < N; i++)
				data_[i] = StringLiteral.value[i] ^ (static_cast<char_type>(K) - static_cast<char_type>(i));
		}

		const auto* decrypt()
		{
			// trick to make sure the compiler doesn't optimize everything away into a strcpy
			auto* ptr = const_cast<volatile char_type*>(data_);

			for (size_t i = 0; i < N; i++)
				ptr[i] ^= (static_cast<char_type>(K) - static_cast<char_type>(i));

			return data_;
		}

	private:
		char_type data_[N]{};
	};
}
