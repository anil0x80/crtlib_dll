#pragma once
#include "cstdint"

namespace util
{
	template <typename T>
	T resolve_rip_relative_offset(uint32_t* offset, uint8_t* next_instruction_address)
	{
		return reinterpret_cast<T>(*offset + next_instruction_address);
	}

	template <typename T>
	T resolve_function_address_e8(uint8_t* p_e8_instruction)
	{
		// call offset is signed 32 bit integer
		int32_t call_offset				= *reinterpret_cast<int32_t*>(p_e8_instruction + 1);
		uint8_t* next_instruction		= p_e8_instruction + 5;
		auto target_function= (next_instruction + call_offset);
		return reinterpret_cast<T>(target_function);
	}
}
