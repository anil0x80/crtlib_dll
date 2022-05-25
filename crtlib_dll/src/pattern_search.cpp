#include "pattern_search.h"
#include <Windows.h>
#include "crt_windows.h"

/* time complexity: O(search_size * pattern_size)*/
// todo optimize
const uint8_t* util::find_pattern(const uint8_t* search_start, size_t search_size, const uint8_t* pattern, size_t pattern_size, uint8_t wildcard)
{
	for (auto p_data = search_start; p_data < search_start + search_size; p_data++)
	{
		/* data is finished */
		if (p_data + pattern_size > search_start + search_size)
			return nullptr;

		/* check for pattern match */
		bool match = true;
		for (size_t i = 0; i < pattern_size; i++)
		{
			if (pattern[i] != wildcard && p_data[i] != pattern[i])
			{
				match = false;
				break;
			}
		}

		if (match)
			return p_data;
	}

	return nullptr;
}

const uint8_t* util::find_code_pattern(const crt::string& module_name, const uint8_t* pattern, size_t pattern_size,
	uint8_t wildcard)
{
	if (module_name.empty())
		return nullptr;

	const auto sections = windows::get_code_blocks_from_pe(GetModuleHandleA(module_name.c_str()));
	return find_code_pattern(sections, pattern, pattern_size, wildcard);
}

const uint8_t* util::find_code_pattern(const crt::vector<windows::memory_block>& blocks, const uint8_t* pattern,
	size_t pattern_size, uint8_t wildcard, uint8_t* from)
{
	for (size_t i = 0; i < blocks.size(); i++)
	{
		auto start = blocks[i].p_data;
		auto size = blocks[i].size;
		if (from)
		{
			auto block_end = blocks[i].p_data + blocks[i].size;
			if (from > block_end) // not exists in this block, we can skip
				continue;
			if (from < block_end && from >= blocks[i].p_data)
			{
				start = from;
				size = (block_end - start);
			}

		}

		auto result = find_pattern(start, size, pattern, pattern_size, wildcard);
		if (result)
			return result;
	}

	return nullptr;
}
