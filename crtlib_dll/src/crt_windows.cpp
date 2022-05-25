#include "crt_windows.h"
#include <Windows.h>
#include <Shlobj.h>

#include "format.hpp"
#include "result.hpp"


crt::string windows::get_current_executable_name()
{
	char path[MAX_PATH];

	const auto path_size = GetModuleFileNameA(nullptr, path, MAX_PATH);
	// path[path_size] == 0, null terminated string
	if (!path_size)
		return crt::string();

	auto last_character = &path[path_size - 1];
	while (*last_character != '\\' && *last_character != '/')
	{
		/* no directories are found */
		if (last_character == path)
			return crt::string(path);

		last_character--;
	}


	return crt::string(last_character + 1);
}

uint8_t* windows::get_current_executable_base()
{
	const auto str_module = get_current_executable_name();
	if (str_module.empty())
		return 0;

	return reinterpret_cast<uint8_t*>(GetModuleHandleA(str_module.c_str()));
}

crt::vector<windows::memory_block> windows::get_code_blocks_from_pe(HMODULE pe_base)
{
	auto result = crt::vector<memory_block>();

	if (pe_base)
	{
		const auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>((uint8_t*)pe_base + reinterpret_cast<IMAGE_DOS_HEADER*>(pe_base)->e_lfanew);
		const auto num_sections = nt_headers->FileHeader.NumberOfSections;

		const auto section_headers = reinterpret_cast<IMAGE_SECTION_HEADER*>(nt_headers + 1);
		for (size_t i = 0; i < num_sections; i++)
		{
			if (section_headers[i].Characteristics & IMAGE_SCN_CNT_CODE || section_headers[i].Characteristics & IMAGE_SCN_MEM_EXECUTE)
			{
				result.push_back({ (uint8_t*)pe_base + section_headers[i].VirtualAddress,
											 section_headers[i].Misc.VirtualSize });
			}
		}
	}

	return result;
}

crt::string windows::get_appdata_roaming_path()
{
	PWSTR p_wpath{};
	auto result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &p_wpath);

	auto ret = windows::narrow_string(p_wpath, crt::wcslen(p_wpath));
	CoTaskMemFree(p_wpath);

	return ret;
}

size_t windows::get_module_size(void* pe)
{
	const auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>((uint8_t*)pe + static_cast<IMAGE_DOS_HEADER*>(pe)->e_lfanew);
	return nt_headers->OptionalHeader.SizeOfImage;
}

bool windows::file_exists(const char* path)
{
	DWORD dwAttrib = GetFileAttributesA(path);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

IoResult windows::read_file(const crt::string& path)
{
	if (!file_exists(path.c_str()))
		return IoResult::error("Specified file doesn't exist!");


	HANDLE h_file = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (h_file == INVALID_HANDLE_VALUE)
	{
		return IoResult::error(crt::format("Error opening file, code:{}", GetLastError()));
	}


	const auto file_size = GetFileSize(h_file, nullptr);
	auto contents_ = crt::vector<uint8_t>(file_size, 0);

	if (!ReadFile(h_file, contents_.data(), static_cast<DWORD>(contents_.size()), nullptr, nullptr)) [[unlikely]]
	{
		CloseHandle(h_file);
		return IoResult::error(crt::format("Error reading file, code:{}", GetLastError()));
	}

	CloseHandle(h_file);
	return IoResult::ok(move(contents_));
}

crt::result<size_t, crt::string> windows::write_file(crt::vector<uint8_t> stream, const crt::string& path,
	t_write_mode mode)
{
	return write_file(stream.data(), stream.size(), path, mode);
}

crt::result<size_t, crt::string> windows::write_file(const void* bytes, size_t size, const crt::string& path,
	t_write_mode mode)
{
	using t_result = crt::result<size_t, crt::string>;

	HANDLE h_file = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
		nullptr, mode == overwrite ? CREATE_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (h_file == INVALID_HANDLE_VALUE)
	{
		return t_result::error(crt::format("Error CreateFileA, code:{}", GetLastError()));
	}

	DWORD written = 0;
	if (!WriteFile(h_file, bytes, size, &written, nullptr))
	{
		CloseHandle(h_file);
		return t_result::error(crt::format("Error WriteFile, code:{}", GetLastError()));
	}

	CloseHandle(h_file);

	return t_result::ok(static_cast<unsigned>(written));
}

crt::result<size_t, crt::string> windows::write_file(crt::vector<crt::string> lines, const crt::string& path,
	t_write_mode mode)
{
	const auto total_size = crt::sum_on([](const crt::string s) {return s.size() + 1; }, lines);
	crt::vector<uint8_t> buffer(total_size, 0);

	size_t buffer_iterator = 0;
	for(auto& l : lines)
	{
		if (l.empty())
		{
			buffer[buffer_iterator++] = '\n';
			continue;
		}

		if (l[l.size() - 1] != '\n')
			l.push_back('\n');

		memcpy(buffer.data() + buffer_iterator, l.c_str(), l.size());
		buffer_iterator += l.size();
	}

	return write_file(buffer.data(), buffer_iterator, path, mode);
}

crt::string windows::narrow_string(const wchar_t* p_wstr, size_t wstr_size)
{
	const auto characters_needed = WideCharToMultiByte(CP_UTF8, 0, p_wstr,
		static_cast<int>(wstr_size), nullptr, 0, nullptr, nullptr);

	/* the resulting string will be null terminated if the p_wstr is null terminated */
	crt::vector buffer((size_t)characters_needed, '\0');

	WideCharToMultiByte(CP_UTF8, 0, p_wstr,
		static_cast<int>(wstr_size), buffer.data(), characters_needed, nullptr, nullptr);

	return crt::string{ buffer.data(), buffer.size() };
}

crt::wstring windows::widen_string(const char* p_str, size_t str_size)
{
	const auto characters_needed = MultiByteToWideChar(CP_UTF8, 0, p_str,
		static_cast<int>(str_size), nullptr, 0);

	crt::vector buffer((size_t)characters_needed, L'\0');

	MultiByteToWideChar(CP_UTF8, 0, p_str,
		static_cast<int>(str_size), buffer.data(), characters_needed);

	return crt::wstring{ buffer.data(), buffer.size() };
}

void windows::print(const char* message)
{
	auto h = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(h, message, static_cast<DWORD>(crt::strlen(message)), nullptr, nullptr);
}

void windows::print_error(const char* message)
{
	auto h = GetStdHandle(STD_ERROR_HANDLE);
	WriteConsoleA(h, message, static_cast<DWORD>(crt::strlen(message)), nullptr, nullptr);
}

crt::vector<crt::base_string<TCHAR>> windows::get_arguments_vector(int argc, TCHAR* argv[])
{
	crt::vector<crt::base_string<TCHAR>> result;
	result.reserve(argc);

	for (int i = 0; i < argc; ++i)
		result.emplace_back(argv[i]);

	return result;
}

crt::vector<crt::string> windows::get_lines(const crt::vector<uint8_t>& bytes)
{
	constexpr uint8_t cr_lf[] = { 0x0D, 0x0A }; 
	constexpr uint8_t lf = 0xA; // \n

	if (bytes.size() < 2)
		return {};

	crt::vector<crt::string> lines;

	//
	auto start = (const char*)&bytes.data()[0];
	for(size_t i =0; i < bytes.size() - 1;)
	{
		const auto current_character = bytes[i];
		const auto next_character = bytes[i + 1];

		auto end = (const char*)&bytes.data()[i];
		if (current_character == 0xD && next_character == 0xA) // \r\n
		{
			lines.emplace_back(start, end - start);
			start = end + 2;
			i += 2;
			continue;
		}

		if(current_character == 0xA)	// \n
		{
			lines.emplace_back(start, end - start);
			start = end + 1;
			i += 1;
			continue;
		}

		++i;
	}

	if(start <= (const char*)&bytes.data()[bytes.size() - 1])
	{
		const auto end = (const char*)&bytes.data()[bytes.size()];
		lines.emplace_back(start, end - start);
	}

	return lines;
}
