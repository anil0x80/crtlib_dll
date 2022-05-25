#include "my_time.h"
#include <Windows.h>

#include "string_utils.hpp"

crt::string crt::get_time_display()   
{
	//[hour.minute.sec.ms]
	SYSTEMTIME st;
	GetLocalTime(&st);

	string result;
	result.reserve(64);
	

	result += "[";
	result += int_to_dec(st.wDay) + "-";
	result += int_to_dec(st.wMonth) + "-";
	result += int_to_dec(st.wYear) + " ";

	result += int_to_dec(st.wHour) + ":";
	result += int_to_dec(st.wMinute) + ":";
	result += int_to_dec(st.wSecond) + ":";
	result += int_to_dec(st.wMilliseconds);
	result += "] ";

	return result;
}

