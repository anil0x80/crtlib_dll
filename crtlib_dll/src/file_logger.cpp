#include "file_logger.h"
#include "my_memory.h"
#include "crt_windows.h"
#include "my_time.h"
#define ENABLE_LOGGING


crt::file_logger::file_logger(const char* file_name)
{
	char temp_buffer[MAX_PATH];
	GetTempPathA(MAX_PATH, temp_buffer);

	crt::string temp_directory{ temp_buffer };

	h_file_ = CreateFileA((temp_directory + file_name).c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

crt::file_logger::~file_logger()
{
	if (h_file_)
		CloseHandle(h_file_);
}

crt::file_logger::file_logger(file_logger&& other) noexcept
{
	swap(*this, other);
}

crt::file_logger& crt::file_logger::operator=(file_logger&& other) noexcept
{
	file_logger dead;
	swap(*this, dead);
	swap(*this, other);
	return *this;
}

void crt::swap(file_logger& p1, file_logger& p2) noexcept
{
	swap(p1.h_file_, p2.h_file_);
	swap(p1.mutex_,  p2.mutex_);
	swap(p1.threaded_buffer_, p2.threaded_buffer_);
	
}

void crt::file_logger::disable()
{
	active_ = false;
}

void crt::file_logger::enable()
{
	active_ = true;
}

// move assignment, invalidate this, swap with other
void crt::file_logger::print(const char* message)
{

	print(message, strlen(message));

}

void crt::file_logger::print(const wchar_t* message)
{

	print(wstring(message));

}


void crt::file_logger::print(const string& message)
{

	print(message.c_str(), message.size());

}

void crt::file_logger::print(void* pointer)
{
	print(reinterpret_cast<uintptr_t>(pointer));

}

void crt::file_logger::print(const wstring& message)
{
	print(windows::narrow_string(message.c_str(), message.size()));

}


void crt::file_logger::print(const void* ptr, size_t size)
{
	if (!active_)
		return;

	lock_guard guard{ mutex_ };

	const auto thread_id = GetCurrentThreadId();
	auto& buffer = threaded_buffer_[thread_id];

	// append message to thread queue
	buffer.reserve(buffer.size() + size);
	buffer.insert(buffer.end(), static_cast<const char*>(ptr), size);

	// parse the input
	while(!buffer.empty())
	{
		const auto message_end = find_first_of(buffer.begin(), buffer.end(), '\n');
		if (message_end == buffer.end())
		{
			// no more data to process
			break;
		}

		// process message 
		auto message = get_time_display();
		message.insert(message.end(), &buffer.front(), message_end - buffer.begin() + 1);
		WriteFile(h_file_, message.c_str(), static_cast<DWORD>(message.size()), nullptr, nullptr);

		// shrink the buffer
		buffer = vector{ &*(message_end + 1), buffer.size() - (message_end - buffer.begin()) - 1 };
	}
}
