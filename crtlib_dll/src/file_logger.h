#pragma once
#include <type_traits>
#include <Windows.h>
#include "my_string.hpp"
#include "mutex.h"
#include "hash_table.hpp"
#include "string_utils.hpp"


namespace crt
{
	class file_logger
	{
	public:
		file_logger() = default;
		explicit file_logger(const char* file_name);
		~file_logger();

		file_logger(const file_logger&) = delete;
		file_logger& operator= (const file_logger&) = delete;

		file_logger(file_logger&& other) noexcept;
		file_logger& operator=(file_logger&& other) noexcept;
		friend void swap(file_logger& p1, file_logger& p2) noexcept;

		template <typename T>
		file_logger& operator<<(T val)
		{
			print(val);
			return *this;
		}

		void disable();
		void enable();

		/* print c string */
		void print(const char* message);
		void print(const wchar_t* message);

		/* print crt string */
		void print(const string& message);
		void print(const wstring& message);
		
		/* print bytes */
		void print(const void* ptr, size_t size);

		/* print integer as hex*/
		template <typename IntType>
		std::enable_if_t<!std::is_pointer_v<IntType>>
		print(IntType value)
		{
			print(crt::int_to_hex(value));
		}
		
		void print(void* pointer); // cast pointer to uintptr_t then print the value


	private:


		HANDLE h_file_{};
		mutex mutex_{};
		hash_map<uint32_t, vector<char>> threaded_buffer_;
		bool active_{ true };
	};
}; 
	

