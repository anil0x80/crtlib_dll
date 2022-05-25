#include "assert.h"

#include "crt_windows.h"
#include "stack_string.hpp"
#include "my_string.hpp"
#include "string_utils.hpp"

void show_assert_message(const char* msg, const char* file, int line)
{
	MessageBoxA(nullptr, (crt::string(DECRYPT_STRING("assertion failed, file: ")) + crt::string(file) + crt::string(DECRYPT_STRING(", line: ")) + crt::string(crt::int_to_dec(line)) + crt::string(DECRYPT_STRING("\nReason:")) + crt::string((msg)) + crt::string("\n")).c_str(), DECRYPT_STRING("AEON"), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}
