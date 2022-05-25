#include "mutex.h"


crt::mutex::mutex()
{
	InitializeCriticalSection(&critical_section_);
}

crt::mutex::mutex(mutex&& rhs)
{
	swap(critical_section_, rhs.critical_section_);
}

crt::mutex& crt::mutex::operator=(mutex&& rhs)
{
	DeleteCriticalSection(&critical_section_);

	swap(critical_section_, rhs.critical_section_);

	return *this;
}


crt::mutex::~mutex()
{
	DeleteCriticalSection(&critical_section_);
}

void crt::mutex::lock()
{
	EnterCriticalSection(&critical_section_);
}

bool crt::mutex::try_lock()
{
	return TryEnterCriticalSection(&critical_section_);
}

void crt::mutex::unlock()
{
	LeaveCriticalSection(&critical_section_);
}

