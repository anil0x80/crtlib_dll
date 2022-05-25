#pragma once
#include "my_memory.h"
#include <utility>
#include <type_traits>

#include "assert.h"

namespace crt
{
	template <typename T, bool IsArray = std::is_array_v<T>>
	class smart_ptr
	{
	public:
		using value_type = std::remove_all_extents_t<T>;
		smart_ptr() = default;

		/* takes ownership of the single heap allocated object T */
		explicit smart_ptr(T* ptr) : ptr_(ptr)
		{
		}

		/* allocates num_elements of objects in heap and takes ownership of them */
		explicit smart_ptr(size_t num_elements)
		{
			ptr_ = new value_type[num_elements]; //static_cast<T*>(crt::alloc(num_elements * sizeof T));
		}

		operator bool()
		{
			return ptr_ != nullptr;
		}

		smart_ptr(const smart_ptr&) = delete;
		smart_ptr& operator= (const smart_ptr&) = delete;

		smart_ptr& operator= (smart_ptr&& other) noexcept
		{
			// move assignment, invalidate this, swap with other
			smart_ptr dead{};
			swap(*this, dead);
			swap(*this, other);

			return *this;
		}

		smart_ptr(smart_ptr&& other) noexcept
		{
			// move constructor
			swap(*this, other);
		}

		friend void swap(smart_ptr& first, smart_ptr& second) noexcept
		{
			crt::swap(first.ptr_, second.ptr_);
		}

		~smart_ptr()
		{
			release();
		}

		void release()
		{
			if (ptr_)
			{
				if constexpr (IsArray)
					delete[] ptr_;
				else
					delete ptr_;
				ptr_ = nullptr;
			}
		}

		value_type* get() const
		{
			return ptr_;
		}

		T* operator->() const
		{
			CRT_ASSERT(ptr_, "smart_ptr dereferencing nullptr!\n");
			return ptr_;
		}

	private:
		value_type* ptr_ = nullptr;
	};

	// construct single object 
	template <typename T, typename... Args>
	std::enable_if_t<!std::is_array_v<T>, smart_ptr <T, false>> make_smart(Args&&... args)
	{
		return smart_ptr<T, false>(new T(std::forward<Args>(args)...));
	}

	// default construct array elements
	template <typename T>
	std::enable_if_t<std::is_array_v<T>, smart_ptr <T, true>> make_smart(size_t num_elements)
	{
		return smart_ptr<T, true>(num_elements);
	}
}

