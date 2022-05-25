#include "to_string.h"

#include "string_utils.hpp"

static crt::string round_float(crt::string str, size_t decimals_after_point)
{
	const auto point = crt::find_first_of(str.begin(), str.end(), '.'); // find the .
	if (point == str.end())
	{
		// 0
		return str;
	}

	const auto number_of_characters_after_point = str.end() - point - 1;

	size_t characters_to_include = decimals_after_point;
	if (decimals_after_point > number_of_characters_after_point)
		characters_to_include = decimals_after_point;

	if (decimals_after_point == 0)
	{
		str.resize(point - str.begin());
		return str;
	}

	str.resize(point + characters_to_include - str.begin() + 1);

	return str;
}

crt::string crt::to_string(const char* message)
{
	return crt::string{ message };
}


crt::string crt::to_string(const string& message)
{
	return message;
}

crt::string crt::to_string(const void* pointer)
{
	return crt::int_to_hex((uintptr_t)pointer);
}
