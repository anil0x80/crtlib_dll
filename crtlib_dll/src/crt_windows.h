#pragma once
#include "my_string.hpp"
#include "my_vector.hpp"
#include <Windows.h>

#include "result.hpp"

typedef crt::vector<uint8_t> Bytes;
typedef crt::result<Bytes, crt::string> IoResult;


#define LOG_CONSOLE(...) windows::print(crt::format(__VA_ARGS__).c_str())
#define ERR_CONSOLE(...) windows::print_error(crt::format(__VA_ARGS__).c_str())

namespace windows
{
	struct memory_block
	{
		uint8_t* p_data;
		size_t size;
	};

	crt::string get_current_executable_name();
	uint8_t* get_current_executable_base();
	crt::vector<memory_block>	get_code_blocks_from_pe(HMODULE pe_base);
	crt::string get_appdata_roaming_path();
	size_t get_module_size(void* pe);
	bool file_exists(const char* path);
	IoResult read_file(const crt::string& path);

	enum t_write_mode
	{
		overwrite,
		append
	};
	crt::result<size_t, crt::string> write_file(crt::vector<uint8_t> stream, const crt::string& path, t_write_mode mode);
	crt::result<size_t, crt::string> write_file(const void* bytes, size_t size, const crt::string& path, t_write_mode mode);
	crt::result<size_t, crt::string> write_file(crt::vector<crt::string> lines, const crt::string& path, t_write_mode mode);

	// this is still needed when writing wide strings to log file
	crt::string narrow_string(const wchar_t* p_wstr, size_t wstr_size);
	crt::wstring widen_string(const char* p_str, size_t str_size);

	// print message to console
	void print(const char* message);
	void print_error(const char* message);


	crt::vector<crt::base_string<TCHAR>> get_arguments_vector(int argc, TCHAR* argv[]);


	// bytes is a file stream.
	crt::vector<crt::string> get_lines(const crt::vector<uint8_t>& bytes);
}