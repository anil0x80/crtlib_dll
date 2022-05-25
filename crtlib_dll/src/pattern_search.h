#pragma once
#include <cstdint>

#include "crt_windows.h"
#include "my_string.hpp"

namespace util
{
	/* find first occurrence of pattern in search */
	const uint8_t* find_pattern(const uint8_t* search_start, size_t search_size, const uint8_t* pattern, size_t pattern_size, uint8_t wildcard);

	/* search module's code sections for pattern match. returns the first occurrence. */
	const uint8_t* find_code_pattern(const crt::string& module_name, const uint8_t* pattern, size_t pattern_size, uint8_t wildcard);

	/* search pattern in specified blocks. */
	const uint8_t* find_code_pattern(const crt::vector<windows::memory_block>& blocks, const uint8_t* pattern, size_t pattern_size, uint8_t wildcard, uint8_t* from = nullptr);


}
