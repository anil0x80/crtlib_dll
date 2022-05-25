#pragma once
#include "hash_table.hpp"

namespace crt
{
	// data structure that contains non-duplicate elements. 
	template <typename T>
	class set
	{
	public:
		set() = default;
		using value_type = T;

		void push_back(T value)
		{
			add(move(value));
		}

		void add(T value)
		{
			table_.insert(move(value), 0);
		}
		void remove(const T& value)
		{
			table_.remove(value);
		}

		bool contains(const T& value) const
		{
			return table_.find_slot(value) != nullptr;
		}

		size_t size() const
		{
			return table_.size();
		}

		bool empty() const
		{
			return size() == 0;
		}

		struct const_iterator
		{
			constexpr static auto tag()
			{
				return forward_iterator_tag{};
			}

			using t_map_iterator = typename hash_map<T, size_t>::iterator;

			const_iterator(t_map_iterator iterator) : table_iterator_(iterator){}

			const T& operator*() const
			{
				auto val = *table_iterator_;
				return *val.first();
			}

			const_iterator& operator++()
			{
				++table_iterator_;
				return *this;
			}
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.table_iterator_ == b.table_iterator_; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.table_iterator_ != b.table_iterator_; }

		private:
			t_map_iterator table_iterator_{};
		};


		const_iterator begin() const
		{
			return const_iterator(table_.begin());
		}


		const_iterator end() const
		{
			return const_iterator(table_.end());
		}
		
	private:
		hash_map<T, size_t> table_{};
	};

}
