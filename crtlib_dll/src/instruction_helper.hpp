#pragma once
#include <cstdint>


namespace util
{
	template <typename T>
	T resolve_relative_offset(const int32_t* offset, const uint8_t* next_instruction_address)
	{
		return (T)(*offset + next_instruction_address);
	}

	template <typename T>
	T resolve_call_target(const uint8_t* p_e8_instruction)
	{
		// call offset is signed 32 bit integer
		return resolve_relative_offset<T>(reinterpret_cast<const int32_t*>(p_e8_instruction + 1),
			p_e8_instruction + 5);
	}

}
