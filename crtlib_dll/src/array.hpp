#pragma once
#include <cstdint>

#include "assert.h"
#include "iterator.hpp"

namespace crt
{
	template<typename T, size_t N>
	class array
	{
	public:
		using value_type = T;

		struct const_iterator
		{
			constexpr static auto tag()
			{
				return random_iterator_tag{};
			}

			friend class array;

			using pointer = const T*;
			using reference = const T&;
			using value_type = T;

			const_iterator(const T* p) : ptr_(p) {}

			reference operator*() const { return *ptr_; }
			pointer operator->() { return ptr_; }

			const_iterator& operator--() { --ptr_; return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

			const_iterator& operator++() { ++ptr_; return *this; }
			const_iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			friend const_iterator operator- (const const_iterator& a, size_t distance) { return const_iterator(a.ptr_ - distance); }
			friend const_iterator operator+ (const const_iterator& a, size_t distance) { return const_iterator(a.ptr_ + distance); }
			friend size_t operator- (const const_iterator& a, const const_iterator& b) { return a.ptr_ - b.ptr_; }
			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.ptr_ == b.ptr_; }
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.ptr_ != b.ptr_; }
			friend bool operator> (const const_iterator& a, const const_iterator& b) { return a.ptr_ > b.ptr_; }
			friend bool operator>= (const const_iterator& a, const const_iterator& b) { return a.ptr_ >= b.ptr_; }
			friend bool operator< (const const_iterator& a, const const_iterator& b) { return a.ptr_ < b.ptr_; }
			friend bool operator<= (const const_iterator& a, const const_iterator& b) { return a.ptr_ <= b.ptr_; }

		private:
			const T* ptr_;
		};
		struct iterator
		{
			constexpr static auto tag()
			{
				return random_iterator_tag{};
			}
			friend class array;

			using value_type = T;
			using pointer = T*;
			using reference = T&;

			iterator(T* p) : ptr_(p) {}

			reference operator*() const { return *ptr_; }
			pointer operator->() { return ptr_; }

			iterator& operator--() { --ptr_; return *this; }
			iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

			iterator& operator++() { ++ptr_; return *this; }
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }


			friend iterator operator- (const iterator& a, size_t distance) { return iterator(a.ptr_ - distance); }
			friend iterator operator+ (const iterator& a, size_t distance) { return iterator(a.ptr_ + distance); }
			friend size_t operator- (const iterator& a, const iterator& b) { return a.ptr_ - b.ptr_; }
			friend bool operator== (const iterator& a, const iterator& b) { return a.ptr_ == b.ptr_; }
			friend bool operator!= (const iterator& a, const iterator& b) { return a.ptr_ != b.ptr_; }
			friend bool operator> (const iterator& a, const iterator& b) { return a.ptr_ > b.ptr_; }
			friend bool operator>= (const iterator& a, const iterator& b) { return a.ptr_ >= b.ptr_; }
			friend bool operator< (const iterator& a, const iterator& b) { return a.ptr_ < b.ptr_; }
			friend bool operator<= (const iterator& a, const iterator& b) { return a.ptr_ <= b.ptr_; }

		private:
			T* ptr_;
		};

		T& operator[](size_t i)
		{
			CRT_ASSERT(i < N, "Array out of bounds access!");

			return buffer_[i];
		}
		const T& operator[](size_t i) const
		{
			CRT_ASSERT(i < N, "Array out of bounds access!");

			return buffer_[i];
		}

		T* data()
		{
			return buffer_;
		}

		const T* data() const
		{
			return buffer_;
		}

		iterator begin()
		{
			return iterator(buffer_);
		}

		const_iterator begin() const
		{
			return  const_iterator(buffer_);
		}

		iterator end()
		{
			return iterator(buffer_ + N);
		}

		const_iterator end() const
		{
			return const_iterator(buffer_ + N);
		}

	private:
		T buffer_[N]{};
	};
}
