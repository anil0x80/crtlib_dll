#pragma once

namespace crt
{
	namespace detail
	{
		template <typename CharType>
		constexpr size_t strlen_imp(const CharType* p_str)
		{
			size_t size = 0;
			while (*p_str++)
			{
				size++;
			}

			return size;
		}

		template <typename CharType>
		constexpr int strncmp_imp(const CharType* str1, const CharType* str2, size_t num)
		{
			for (size_t i = 0; i < num; ++i)
			{
				if (str1[i] != str2[i])
					return str1[i] > str2[i] ? 1 : -1;

				// character equal
				if (str1[i] == 0)
					return 0;
			}

			return 0;
		}

		template <typename CharType>
		constexpr int strcmp_imp(const CharType* str1, const CharType* str2)
		{
			for (size_t i = 0; ; i++)
			{
				if (str1[i] != str2[i])
					return str1[i] > str2[i] ? 1 : -1;
				else if (str1[i] == 0) // we reached the end, str1[i] == str2[i]
					return 0;
			}
		}

		template <typename CharType>
		constexpr const CharType* strstr_imp(const CharType* p_string, const CharType* p_substring)
		{
			/* check if both strings are empty */
			if (p_string[0] == 0 && p_string[0] == p_substring[0])
				return p_string;

			const size_t size_of_substring = strlen_imp<CharType>(p_substring) * sizeof(CharType);
			for (size_t i = 0; ; i++)
			{
				if (p_string[i] == 0)
					break;

				/* check for pattern match. notice that it is safe to use this
				 * because the p_string is null terminated, and we will never
				 * read bad memory, memcmp will return as soon as it sees the null terminator.
				 */
				if (!memcmp(p_string + i, p_substring, size_of_substring))
					return p_string + i;
			}

			return nullptr;
		}

		template <typename CharType>
		constexpr CharType* strstr_imp(CharType* p_string, const CharType* p_substring)
		{
			return const_cast<CharType*>(strstr_imp<CharType>(static_cast<const CharType*>(p_string), p_substring));
		}
	}


	/* define C style string helper functions */
	constexpr inline size_t strlen(const char* p_str)
	{
		return detail::strlen_imp(p_str);
	}

	constexpr inline size_t wcslen(const wchar_t* p_str)
	{
		return detail::strlen_imp(p_str);
	}

	constexpr inline int strncmp(const char* str1, const char* str2, size_t num)
	{
		return detail::strncmp_imp(str1, str2, num);
	}

	constexpr inline int wcsncmp(const wchar_t* str1, const wchar_t* str2, size_t num)
	{
		return detail::strncmp_imp(str1, str2, num);
	}

	constexpr inline int strcmp(const char* str1, const char* str2)
	{
		return detail::strcmp_imp(str1, str2);
	}

	constexpr inline int wcscmp(const wchar_t* str1, const wchar_t* str2)
	{
		return detail::strcmp_imp(str1, str2);
	}

	constexpr inline const char* strstr(const char* p_string, const char* p_substring)
	{
		return detail::strstr_imp(p_string, p_substring);
	}

	constexpr inline char* strstr(char* p_string, const char* p_substring)
	{
		return detail::strstr_imp(p_string, p_substring);
	}

	constexpr inline const wchar_t* wcsstr(const wchar_t* p_string, const wchar_t* p_substring)
	{
		return detail::strstr_imp(p_string, p_substring);
	}

	constexpr inline wchar_t* wcsstr(wchar_t* p_string, const wchar_t* p_substring)
	{
		return detail::strstr_imp(p_string, p_substring);
	}

	template<typename T>
	constexpr bool is_space(T c)
	{
		return false;
	}

	template<>
	constexpr bool is_space(wchar_t c)
	{
		return c == L' ' || c == L'\t';
	}

	template<>
	constexpr bool is_space(char c)
	{
		return c == ' ' || c == '\t';
	}
}