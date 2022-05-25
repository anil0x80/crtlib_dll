#pragma once
#include "my_string.hpp"
#include "maybe.hpp"
#include "algorithm.hpp"

namespace crt
{
	namespace detail
	{
		template <typename CharType, typename IntType>
		base_string<CharType> int_to_hex_imp(IntType integer, size_t character_count = sizeof(IntType) << 1)
		{
			if constexpr (std::is_same_v<IntType, bool>)
				return integer ? "1" : "0";

			crt::base_string<CharType> str(character_count, 0);
			for (size_t i = 0, j = (character_count - 1) * 4; i < character_count; ++i, j -= 4)
			{
				const CharType digit_value = (integer >> j) & 0x0f;
				const CharType digit_character = digit_value < 10 ? 0x30 + digit_value : 0x41 + (digit_value - 10);
				str[i] = digit_character;
			}

			auto start = crt::find_first_not_of(str.begin(), str.end(), '0');
			if (start == str.end() && str[str.size() - 1] == '0')
				start = &str[str.size() - 1];
			return start != str.end() ? crt::slice(str, start, str.end()) : str;
		}

		template <typename CharType, typename IntType>
		crt::base_string<CharType> int_to_dec_imp(IntType integer)
		{
			if constexpr (std::is_same_v<IntType, bool>)
				return integer ? "1" : "0";

			crt::base_string<CharType> resulting_digits;

			bool negative = false;
			if constexpr (std::is_signed_v<IntType>)
			{
				// handle negative number
				if (integer < 0)
				{
					negative = true;
					integer = -integer;
				}
			}

			IntType quotient, remainder;
			while (integer)
			{
				quotient = integer / 10;
				remainder = integer % 10;		// digits

				resulting_digits.push_back(static_cast<CharType>(remainder) + 0x30);

				integer = quotient;
			}

			if (negative)
			{
				resulting_digits.push_back(static_cast<CharType>(0x2D));
			}

			// integer was 0.
			if (resulting_digits.empty())
			{
				resulting_digits.push_back(0x30); // push 0
			}
			else
			{
				crt::reverse(resulting_digits.begin(), resulting_digits.end());
			}

			return resulting_digits;
		}
	}

	inline bool is_digit(uint16_t character)
	{
		return (character >= 48 && character <= 57) || (character >= 65 && character <= 70);
	}

	inline uint8_t get_digit_value_of_character(uint16_t digit)
	{
		/* ascii range [48..57] -> [0..9] */
		if (digit >= 48 && digit <= 57)
		{
			return static_cast<uint8_t>(digit) - 48;
		}

		/* ascii range [65..70] -> [10..15]*/
		if (digit >= 65 && digit <= 70)
		{
			return static_cast<uint8_t>(digit) - 65 + 10;
		}

		/* bad input */
		return 0;
	}

	template <typename IntType>
	string int_to_hex(IntType integer)
	{
		return detail::int_to_hex_imp<char>(integer);
	}

	template <typename IntType>
	wstring int_to_hex_wide(IntType integer)
	{
		return detail::int_to_hex_imp<wchar_t>(integer);
	}

	template <typename IntType>
	string int_to_dec(IntType integer)
	{
		return detail::int_to_dec_imp<char>(integer);
	}

	template <>
	inline string int_to_dec(bool integer)
	{
		return integer ? "1" : "0";
	}

	template <typename IntType>
	wstring int_to_dec_wide(IntType integer)
	{
		return detail::int_to_dec_imp<wchar_t>(integer);
	}


	template <typename CharType>
	crt::maybe<uint32_t> string_to_int(const CharType* p_string, uint32_t base = 16)
	{
		const auto size_of_string = static_cast<int>(detail::strlen_imp(p_string));

		uint32_t value = 0;
		size_t digit_idx = 0;
		for (int i = size_of_string - 1; i >= 0; --i)
		{
			if (!is_digit(p_string[i]))
				return nothing<uint32_t>();

			value += get_digit_value_of_character(p_string[i]) * exponent(base, digit_idx);

			digit_idx++;
		}

		return just<uint32_t>(value);
	}

	
	inline uint8_t hex_to_uint8_unwrap(const crt::string& str)
	{
		return static_cast<uint8_t>(*string_to_int<char>(str.c_str(), 16));
	}
}
