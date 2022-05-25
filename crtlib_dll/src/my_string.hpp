#pragma once
#include <cstdint>
#include "array.hpp"
#include "iterator.hpp"
#include "my_memory.h"
#include "my_math.hpp"
#include "heap_allocator.h"
#include "c_string.hpp"
#include "hash.hpp"

#include "algorithm.hpp"
namespace crt
{
	
	// strings less than 16 characters will not be heap allocated
	constexpr size_t max_small_string_size = 15;

	template <typename CharType>
	class base_string
	{
	public:
		using value_type = CharType;

		struct iterator
		{
			constexpr static auto tag()
			{
				return random_iterator_tag{};
			}

			friend class base_string;

			using value_type = CharType;
			using pointer = CharType*;
			using reference = CharType&;

			iterator(CharType* p) : ptr_(p) {}

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
			CharType* ptr_;
		};
		struct const_iterator
		{
			constexpr static auto tag()
			{
				return random_iterator_tag{};
			}

			friend class base_string;

			using value_type = CharType;
			using pointer = const CharType*;
			using reference = const CharType&;

			const_iterator(const CharType* p) : ptr_(p) {}
			const_iterator(iterator it) : ptr_(it.ptr_) {}

			reference operator*() const { return *ptr_; }
			pointer operator->() { return ptr_; }

			const_iterator& operator--() { --ptr_; return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

			const_iterator& operator++() { ++ptr_; return *this; }
			const_iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			friend const_iterator operator- (const const_iterator& a, size_t distance) { return const_iterator(a.ptr_ - distance); }
			friend const_iterator operator+ (const const_iterator& a, size_t distance) { return const_iterator(a.ptr_ + distance); }
			friend size_t operator- (const const_iterator& a, const const_iterator& b) { return a.ptr_ - b.ptr_; };
			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.ptr_ == b.ptr_; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.ptr_ != b.ptr_; }
			friend bool operator> (const const_iterator& a, const const_iterator& b) { return a.ptr_ > b.ptr_; }
			friend bool operator>= (const const_iterator& a, const const_iterator& b) { return a.ptr_ >= b.ptr_; }
			friend bool operator< (const const_iterator& a, const const_iterator& b) { return a.ptr_ < b.ptr_; }
			friend bool operator<= (const const_iterator& a, const const_iterator& b) { return a.ptr_ <= b.ptr_; }

		private:
			const CharType* ptr_;
		};

		constexpr base_string() = default;

		/* from null terminated c string */
		constexpr base_string(const CharType* p_null_terminated_str)
			: base_string(p_null_terminated_str, detail::strlen_imp<CharType>(p_null_terminated_str))
		{

		}

		/* non null terminated string */
		constexpr base_string(const CharType* p_str, size_t size) : base_string()
		{
			assign(p_str, size);
		}

		constexpr void assign(const CharType* p_str, size_t size)
		{
			if (size <= max_small_string_size)
			{
				// use the internal buffer
				memcpy(small_buffer_.data(), p_str, size * sizeof(CharType));
				small_buffer_[size] = 0;
				size_ = size;
				capacity_ = max_small_string_size + 1;
				buffer_ = nullptr;	// prevent double free
			}
			else
			{
				size_ = size;
				capacity_ = (size_t)next_power_of_2(static_cast<unsigned long>(size + 1));
				buffer_ = static_cast<CharType*>(alloc(capacity_ * sizeof(CharType)));
				memcpy(buffer_, p_str, size * sizeof(CharType));
				buffer_[size] = 0;
			}
		}

		/* crate a string that contains size repetitions of character.*/
		constexpr base_string(size_t size, CharType character) : size_(size), capacity_(size + 1)
		{
			if (size <= max_small_string_size)
			{
				// use the internal buffer
				for (size_t i = 0; i < size_; ++i)
					small_buffer_[i] = character;
				small_buffer_[size] = 0;
				capacity_ = max_small_string_size + 1;
			}
			else
			{
				buffer_ = static_cast<CharType*>(alloc(capacity_ * sizeof(CharType)));
				for (size_t i = 0; i < size_; ++i)
					buffer_[i] = character;
				buffer_[size] = 0;
			}
		}

		// copy constructor
		constexpr base_string(const base_string& rhs) : base_string(rhs.c_str(), rhs.size())
		{

		}

		// copy assignment operator
		constexpr base_string& operator=(const base_string& rhs)
		{
			if (this != &rhs)
			{
				free(buffer_);
				assign(rhs.c_str(), rhs.size());
			}
			return *this;
		}

		// move constructor
		constexpr base_string(base_string&& rhs) noexcept : base_string()
		{
			crt::swap(buffer_, rhs.buffer_);
			crt::swap(size_, rhs.size_);
			crt::swap(capacity_, rhs.capacity_);
			crt::swap(small_buffer_, rhs.small_buffer_);
		}

		// move assignment operator
		constexpr base_string& operator=(base_string&& rhs) noexcept
		{
			free(buffer_);
			buffer_ = nullptr;
			size_ = 0;
			capacity_ = 0;
			// todo not resetting small_buffer_, is problem?

			crt::swap(buffer_, rhs.buffer_);
			crt::swap(size_, rhs.size_);
			crt::swap(capacity_, rhs.capacity_);
			crt::swap(small_buffer_, rhs.small_buffer_);

			return *this;
		}

		// destructor
		~base_string()
		{
			if (buffer_)
			{
				free(buffer_);
				buffer_ = nullptr;
			}

		}

		constexpr const CharType& operator[](size_t index) const
		{
			//CRT_ASSERT(index < size(), "String out of bounds access!");

			return c_str()[index];
		}

		constexpr CharType& operator[](size_t index)
		{
			//CRT_ASSERT(index < size(), "String out of bounds access!");

			return c_str()[index];
		}

		constexpr iterator begin()
		{
			return iterator(c_str());
		}

		constexpr const_iterator begin() const
		{
			return const_iterator(c_str());
		}

		constexpr iterator end()
		{
			return iterator(c_str() + size_);
		}

		constexpr const_iterator end() const
		{
			return const_iterator(c_str() + size_);
		}

		constexpr friend bool operator==(const base_string& s1, const base_string& s2)
		{
			return !detail::strcmp_imp<CharType>(s1.c_str(), s2.c_str());
		}

		constexpr friend bool operator==(const base_string& s1, const CharType* s2)
		{
			return !detail::strcmp_imp<CharType>(s1.c_str(), s2);
		}

		constexpr friend bool operator==(const CharType* s1, const base_string& s2)
		{
			return s2 == s1;
		}

		constexpr void reserve(size_t new_capacity)
		{
			if (capacity_ < new_capacity)
			{
				capacity_ = new_capacity;
				buffer_ = buffer_ ? static_cast<CharType*>(realloc(buffer_, capacity_ * sizeof(CharType)))
					: static_cast<CharType*>(alloc(capacity_ * sizeof(CharType)));
			}
		}

		constexpr void resize(size_t new_size)
		{
			const auto old_size = size();
			size_ = new_size;

			if (new_size < old_size)
			{
				// shrink
				c_str()[old_size] = 0;
				return;
			}

			if (size_ + 1 <= capacity_)
				return;

			const bool small_to_heap_transition = old_size <= max_small_string_size;

			while (size_ + 1 > capacity_)
				capacity_ *= 2;

			buffer_ = buffer_ ? static_cast<CharType*>(realloc(buffer_, capacity_ * sizeof(CharType)))
				: static_cast<CharType*>(alloc(capacity_ * sizeof(CharType)));

			if (small_to_heap_transition)
			{
				// copy memory from buffer to heap
				memcpy(buffer_, small_buffer_.data(), (max_small_string_size + 1) * sizeof(CharType));
			}
		}

		constexpr base_string& operator+=(const base_string& rhs)
		{
			// append rhs into this
			const auto old_size = size();
			resize(size() + rhs.size());

			for (size_t i = 0; i < rhs.size(); ++i)
				c_str()[old_size + i] = rhs[i];

			c_str()[size_] = 0;
			return *this;
		}

		constexpr friend base_string operator+(base_string lhs, const base_string& rhs)
		{
			lhs += rhs;
			return lhs;
		}

		constexpr base_string& operator+=(const CharType* rhs)
		{
			// append rhs into this
			const auto rhs_size = detail::strlen_imp<CharType>(rhs);
			const auto old_size = size();
			resize(size() + rhs_size);

			for (size_t i = 0; i < rhs_size; ++i)
				c_str()[old_size + i] = rhs[i];

			c_str()[size_] = 0;
			return *this;
		}

		constexpr friend base_string operator+(base_string lhs, const CharType* rhs)
		{
			lhs += rhs;
			return lhs;
		}

		constexpr void push_back(CharType character)
		{
			resize(size_ + 1);
			c_str()[size_ - 1] = character;
			c_str()[size_] = 0;
		}


		constexpr void pop_back()
		{
			CRT_ASSERT(size_, "String empty pop!");
			size_ -= 1;
			c_str()[size_] = 0;
		}

		constexpr void insert(iterator pos, CharType ch)
		{
			auto idx = pos.ptr_ - c_str();
			if (idx >= size())
				return push_back(ch);

			const size_t chars_to_shift = size() - idx;
			resize(size_ + 1);

			memmove(c_str() + idx + 1, c_str() + idx, chars_to_shift * sizeof(CharType));


			c_str()[idx] = ch;
			c_str()[size_] = 0;
		}

		// insert all items in [it_begin..it_end) to the pos
		constexpr void insert(iterator pos, const_iterator it_begin, const_iterator it_end)
		{
			size_t size = it_end - it_begin;
			return insert(pos, it_begin.ptr_, size);
		}

		constexpr void insert(iterator pos, const CharType* origin, size_t count)
		{
			size_t idx = pos - c_str();
			if (idx > size())
				idx = size();

			const size_t chars_to_shift = size() - idx;
			resize(size_ + count);

			if (chars_to_shift)
				memmove(c_str() + idx + count, c_str() + idx, chars_to_shift * sizeof(CharType));

			memcpy(c_str() + idx, origin, count * sizeof(CharType));
			c_str()[size_] = 0;
		}

		[[nodiscard]] constexpr const CharType* c_str() const
		{
			if (buffer_)
				return buffer_;

			return &small_buffer_[0];
		}

		[[nodiscard]] constexpr CharType* c_str()
		{
			if (buffer_)
				return buffer_;

			return &small_buffer_[0];
		}

		[[nodiscard]] constexpr CharType& front()
		{
			CRT_ASSERT(!empty(), "String empty dereference!");
			return c_str()[0];
		}


		[[nodiscard]] constexpr CharType& back()
		{
			CRT_ASSERT(!empty(), "String empty dereference!");
			return c_str()[size() - 1];
		}

		[[nodiscard]] constexpr size_t size() const
		{
			return size_;
		}

		[[nodiscard]] constexpr bool empty() const
		{
			return size_ == 0;
		}

		constexpr bool starts_with(const base_string<CharType>& str) const
		{
			if (size() < str.size())
				return false;

			return !detail::strncmp_imp(c_str(), str.c_str(), str.size());
		}

		constexpr bool starts_with(const CharType* str) const
		{
			size_t len = detail::strlen_imp(str);
			if (size() < len)
				return false;

			return !detail::strncmp_imp(c_str(), str, len);
		}

		// returns first occurrence of s in this, nullptr if can't find
		constexpr CharType* find(const CharType* s, size_t pos = 0)
		{
			if (!c_str())
				return nullptr;

			return detail::strstr_imp(c_str() + pos, s);
		}

		constexpr bool contains(const CharType* s)
		{
			return find(s) != nullptr;
		}

		constexpr bool contains(const base_string<CharType>& s)
		{
			return contains(s.c_str());
		}

		constexpr CharType* find(const base_string<CharType>& s, size_t pos = 0)
		{
			if (!c_str() || !s.c_str())
				return nullptr;

			return detail::strstr_imp(c_str() + pos, s.c_str());
		}

		constexpr base_string<CharType> ltrim() const
		{
			const auto start_idx = find_first_idx_by([](CharType c)
			{
				return !is_space(c);
			}, *this);

			if(is_just(start_idx))
			{
				return crt::slice_idx(*this, *start_idx, size());
			}

			return base_string<CharType>();
		}

		constexpr base_string<CharType> rtrim() const
		{
			const auto end_idx = find_last_idx_by([](CharType c)
				{
					return !is_space(c);
				}, *this);

			if (is_just(end_idx))
			{
				return crt::slice_idx(*this, 0, (*end_idx + 1));
			}

			return base_string<CharType>();
		}

		iterator erase(const iterator& it)
		{
			auto position = it.ptr_;

			if (position == end())
				return end();

			const size_t num_elements_to_shift = end() - position - 1;

			--size_;

			memmove(position, position + 1, num_elements_to_shift * sizeof(CharType));

			return iterator(position);
		}

		iterator erase_idx(size_t idx)
		{
			return erase(begin() + idx);
		}

	private:
		// the internal string is always null terminated, size does not include the null terminating character 
		CharType* buffer_{};
		// 16 bytes
		crt::array<CharType, max_small_string_size + 1> small_buffer_;

		size_t    size_{};
		size_t    capacity_{ max_small_string_size + 1 };
	};

	typedef base_string<char>		string;
	typedef base_string<wchar_t>	wstring;


	template <typename T>
	constexpr size_t get_hash(const base_string<T>& str)
	{
		return fnv_1a(reinterpret_cast<const uint8_t*>(str.c_str()), str.size());
	}
}
