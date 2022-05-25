#pragma once
#include "heap_allocator.h"
#include "my_memory.h"
#include <type_traits>

#include "assert.h"
#include "iterator.hpp"
#include "hash.hpp"
#include "my_math.hpp"


namespace crt
{
	template <typename T>
	class vector
	{
	public:
		using value_type = T;

		struct const_iterator
		{
			constexpr static auto tag()
			{
				return random_iterator_tag{};
			}

			friend class vector;

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
			friend class vector;

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

		vector() : begin_(nullptr), end_(nullptr), end_capacity_(nullptr)
		{

		}

		explicit vector(const size_t initial_capacity) : vector()
		{
			reserve(initial_capacity);
		}

		// vector of count copies of element
		explicit vector(const size_t count, const T& element) : vector()
		{
			reserve(count);

			for (size_t i = 0; i < count; ++i)
				*end_++ = element;
		}

		vector(std::initializer_list<T> args)
		{
			reserve(args.size());
			for (const T& elem : args)
			{
				push_back(elem);
			}
		}

		// copy from raw array
		explicit vector(const T* arr, const size_t count) : vector()
		{
			reserve(count);

			if constexpr (std::is_trivially_copyable_v<T>)
			{
				const auto size_bytes = count * sizeof(T);

				memmove(end_, arr, size_bytes);

				end_ += count;
			}
			else
			{
				for (size_t i = 0; i < count; ++i)
				{
					new(end_++) T(arr[i]);
				}
			}
		}

		~vector()
		{
			erase();
			crt::free(begin_, alignof(T));
		}

		vector(const vector& rhs) : vector()
		{
			const auto rhs_cap = rhs.capacity();
			if(rhs_cap)
				set_capacity(rhs_cap);

			if constexpr (std::is_trivially_copyable_v<T>)
			{
				const auto size_bytes = rhs.size() * sizeof(T);

				memmove(end_, rhs.data(), size_bytes);

				end_ += rhs.size();
			}
			else
			{
				for (const auto& val : rhs)
				{
					new(end_++) T(val);
				}
			}
		}

		vector& operator=(vector other)
		{
			swap(*this, other);
			return *this;
		}

		vector(vector&& other) noexcept : vector()
		{
			swap(*this, other);
		}

		friend void swap(vector& lhs, vector& rhs) noexcept
		{
			swap(lhs.begin_, rhs.begin_);
			swap(lhs.end_, rhs.end_);
			swap(lhs.end_capacity_, rhs.end_capacity_);
		}

		// reserves the vector [if needed] so that the vector has at least specified capacity.
		void reserve(size_t needed_capacity)
		{
			if (capacity() < needed_capacity)
				set_capacity((size_t)next_power_of_2(static_cast<unsigned long>(needed_capacity)));
		}

		// destroys all elements of the vector, sets size to 0. does not affect capacity
		void erase()
		{
			for (auto it = begin_; it != end_; ++it)
			{
				it->~T();
			}
			end_ = begin_;
		}

		const T& operator[](size_t index) const
		{
			CRT_ASSERT(index < size(), "Vector out of bounds access!");

			return begin_[index];
		}

		T& operator[](size_t index)
		{
			CRT_ASSERT(index < size(), "Vector out of bounds access!");

			return begin_[index];
		}

		[[nodiscard]] const T* data() const
		{
			return begin_;
		}

		[[nodiscard]] T* data()
		{
			return begin_;
		}

		[[nodiscard]] size_t size() const
		{
			return end_ - begin_;
		}

		[[nodiscard]] size_t capacity() const
		{
			return end_capacity_ - begin_;
		}

		[[nodiscard]] bool empty() const
		{
			return end_ == begin_;
		}

		// add item into ptr_, increase size. if necessary, increase capacity
		void push_back(T item)
		{
			if (end_ == end_capacity_)
				grow();

			new(end_) T(crt::move(item));
			++end_;
		}

		// avoids move constructors by constructing the element in place
		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			if (end_ == end_capacity_)
				grow();

			new(end_) T(std::forward<Args>(args)...);
			++end_;
		}

		void pop_back()
		{
			--end_;
			end_->~T();
		}

		void erase(const T& val)
		{
			for(auto it = begin_; it != end_; ++it)
			{
				if (*it == val)
				{
					erase(it);
					return;
				}
			}
		}

		iterator erase(const iterator& range_begin, const iterator& range_end)
		{
			const auto begin_position = range_begin.ptr_;
			if (begin_position == end_)
				return end_;

			const auto delete_size = range_end - range_begin;

			for(auto it = range_begin; it != range_end; ++it)
			{
				it.ptr_->~T();// destruct element
			}

			const size_t num_elements_to_shift = end_ - begin_position - delete_size;
			end_ -= delete_size;

			memmove(begin_position, begin_position + delete_size, num_elements_to_shift * sizeof(T));

			return iterator(begin_position);
		}

		iterator erase(const iterator& it)
		{
			auto position = it.ptr_;

			if (position == end_)
				return end_;

			const size_t num_elements_to_shift = end_ - position - 1;

			--end_;
			position->~T(); // destruct element

			memmove(position, position + 1, num_elements_to_shift * sizeof(T));

			return iterator(position);
		}

		iterator erase_idx(size_t idx)
		{
			return erase(begin() + idx);
		}

		// O(1) erase that doesn't preserve the order of elements.
		iterator unstable_erase(const iterator& it)
		{
			if (it.ptr_ == end_)
				return end_;

			crt::swap(*it, back());
			pop_back();
		
			return it;
		}

		void unstable_erase(const T& val)
		{
			for (auto it = begin_; it != end_; ++it)
			{
				if (*it == val)
				{
					unstable_erase(it);
					return;
				}
			}
		}

		iterator unstable_erase_idx(const size_t idx)
		{
			return unstable_erase(begin() + idx);
		}

		T&& pop(const iterator& it)
		{
			auto&& elem = crt::move(*it);

			erase(it);

			return move(elem);
		}

		T&& pop_idx(size_t idx)
		{
			return move(pop(begin() + idx));
		}

		T&& pop_unstable(const iterator& it)
		{
			auto&& elem = crt::move(*it);

			unstable_erase(it);

			return move(elem);
		}

		T&& pop_unstable_idx(size_t idx)
		{
			return move(pop_unstable(begin() + idx));
		}

		// insert single element into position. 
		void insert(const iterator& it, T item)
		{
			const size_t num_elements_to_shift = end_ - it.ptr_;

			T* position;
			if (end_ == end_capacity_)
			{
				const size_t position_index = it.ptr_ - begin_;
				grow();
				position = begin_ + position_index;
			}
			else
			{
				position = it.ptr_;
			}

			memmove(position + 1, position, num_elements_to_shift * sizeof(T));
			new(position) T(crt::move(item));
			++end_;
		}

		// insert another vector into position
		void insert(const iterator& it, const vector<T>& rhs)
		{
			return insert(it, rhs.data(), rhs.size());
		}

		// insert from raw array
		void insert(const iterator& it, const T* source_array, size_t source_element_count)
		{
			if (source_element_count == 0)
				return;

			const size_t num_elements_to_shift = end_ - it.ptr_;

			const size_t required_capacity = size() + source_element_count;

			T* position;
			if (capacity() < required_capacity)
			{
				const size_t position_index = it.ptr_ - begin_;
				set_capacity((size_t)next_power_of_2(static_cast<unsigned long>(required_capacity)));
				position = begin_ + position_index;
			}
			else
			{
				position = it.ptr_;
			}

			end_ += source_element_count;

			// shift elements forward to make space for array
			memmove(position + source_element_count, position, num_elements_to_shift * sizeof(T));

			// write array to vector at position
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				const auto size_bytes = source_element_count * sizeof(T);
				memmove(position, source_array, size_bytes);
			}
			else
			{
				for (size_t i = 0; i < source_element_count; i++)
				{
					// invoke copy constructor of T
					new(position++) T(source_array[i]);
				}
			}
		}

		T& front()
		{
			return *begin_;
		}

		const T& front() const
		{
			return *begin_;
		}


		auto begin()
		{
			return iterator(begin_);
		}

		auto begin() const
		{
			return const_iterator(begin_);
		}

		auto end()
		{
			return iterator(end_);
		}

		auto end() const
		{
			return const_iterator(end_);
		}

		T& back()
		{
			return *(end_ - 1);
		}

		const T& back() const
		{
			return *(end_ - 1);
		}

		bool contains(const T& element) const
		{
			for(auto it = begin_; it != end_; ++it)
			{
				if (*it == element)
					return true;
			}

			return false;
		}

	private:
		void grow()
		{
			const auto cap = capacity();
			const auto next_capacity = cap ? cap * 2 : 4; // initial capacity is 4
			set_capacity(next_capacity);
		}

		void set_capacity(const size_t new_capacity)
		{
			//CRT_ASSERT(new_capacity > capacity(), "Vector shrink detected!");

			const auto current_size = size();

			// the resize does not invoke any default constructors
			if (begin_)
			{
				begin_ = static_cast<T*>(realloc(static_cast<void*>(begin_), new_capacity * sizeof T, alignof(T)));
			}
			else
			{
				begin_ = static_cast<T*>(alloc(new_capacity * sizeof T, alignof(T)));
			}

			end_ = begin_ + current_size;
			end_capacity_ = begin_ + new_capacity;

		}

		T* begin_{};
		T* end_{};
		T* end_capacity_{};
	};

	template <typename T>
	constexpr size_t get_hash(const vector<T>& vec)
	{
		size_t seed = 0;
		for (const auto& element : vec)
		{
			hash_combine(seed, element);
		}
		return seed;
	}
}
