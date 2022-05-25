#pragma once
#include "crt_windows.h"


namespace crt
{
	// lockable mt protector
	class mutex
	{
	public:
		mutex();
		mutex(const mutex&) = delete;
		mutex& operator=(const mutex&) = delete;
		mutex(mutex&&);
		mutex& operator=(mutex&&);
		~mutex();

		// wait and lock
		void lock();

		// returns false if the mutex is already locked, otherwise locks the mutex
		bool try_lock();

		// unlock the mutex
		void unlock();

		
	private:
		CRITICAL_SECTION critical_section_{};
	};

	template <typename T>
	class lock_guard
	{
	public:
		lock_guard() = default;

		lock_guard(T& m) : m_(m)
		{
			m_.lock();
		}

		lock_guard(lock_guard&& rhs) = delete;
		lock_guard& operator=(lock_guard&& rhs) = delete;
		lock_guard(const lock_guard&) = delete;
		lock_guard& operator=(const lock_guard&) = delete;

		~lock_guard()
		{
			m_.unlock();
		}

	private:
		T& m_{};
	};
}
