#pragma once
#include "my_string.hpp"
#include "string_utils.hpp"

namespace crt
{
	/* print c string */
	crt::string to_string(const char* message);

	/* print crt string */
	crt::string to_string(const string& message);


	/* print integer as hex*/
	template <typename IntType>
	std::enable_if_t<!std::is_pointer_v<IntType>, crt::string>
		to_string(IntType value)
	{
		return to_string(crt::int_to_hex(value));
	}

	crt::string to_string(const void* pointer);

}
