#pragma once
#include "my_memory.h"
#include "hash.hpp"
#include "maybe.hpp"
#include "smart_ptr.hpp"

namespace crt
{
	template <typename K, typename V>
	class hash_map
	{
		enum slot_state : uint8_t
		{
			null = 0, // unoccupied
			deleted = 1, // deleted
			occupied = 2 // occupied
		};

		class table_slot
		{
		public:
			// at the start of the lifetime of the slot, key_ and value_ are in uninitialized states.
			// that is, no default constructors from type K or type V are called. 
			table_slot() : state_(null), key_{}, value_{}
			{

			}

			const K& get_key() const
			{
				return key_;
			}

			const V& get_value() const
			{
				return value_;
			}

			V& get_value()
			{
				return value_;
			}

		private:

			// remove the slot, destructing its key and value
			void remove()
			{
				state_ = deleted; // mark as deleted
				destruct_key_and_value();
			}

			// explicitly call destructors for key and value
			void destruct_key_and_value()
			{
				(&key_)->~K();
				(&value_)->~V();
			}

			friend class hash_map;

			// 1 byte -> aligned to 4
			slot_state state_;		// current state of the slot

			// 4 bytes
			size_t hashcode_key_; // cached hashcode of the key for fast comparison

			// K bytes
			K key_{};

			// V bytes
			V value_{};
		};

	public:
		struct iterator
		{
			iterator(table_slot* p, table_slot* table_end) : slot_(p), end_(table_end) {}


			crt::pair<const K*, V*> operator*() const
			{
				return crt::make_pair(&slot_->get_key(), &slot_->get_value());
			}

			iterator& operator++()
			{
				// increment to next entry 
				++slot_;

				// go to next occupied slot
				while (slot_ < end_ && slot_->state_ != occupied)
				{
					++slot_;
				}

				return *this;
			}
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const iterator& a, const iterator& b) { return a.slot_ == b.slot_; };
			friend bool operator!= (const iterator& a, const iterator& b) { return a.slot_ != b.slot_; }

		private:
			table_slot* slot_;
			table_slot* end_;
		};

		constexpr hash_map() : table_(nullptr), table_size_(0), table_capacity_(0)
		{

		}

		constexpr hash_map(std::initializer_list<crt::pair<K, V>> args)
		{
			for (const auto& elem : args)
			{
				insert(elem.first(), elem.second());
			}
		}

		// initial_capacity is rounded to nearest prime 
		constexpr explicit hash_map(size_t initial_capacity) : hash_map()
		{
			while (!is_prime(++initial_capacity))
			{

			}

			resize(initial_capacity);
		}

		~hash_map()
		{
			// will invoke the default destructors of the slot, which will invoke the destructors of
			// uninitialized_object, which will invoke the destructor of key/value, if they exist for a given slot.
			if (table_)
				delete[] table_;
		}

		constexpr hash_map(hash_map&& other) noexcept : table_(nullptr), table_size_(0), table_capacity_(0)
		{
			swap(*this, other);
		}

		constexpr hash_map(const hash_map& other) : table_(nullptr), table_size_(0), table_capacity_(0)
		{
			if (other.table_capacity_)
				resize(other.table_capacity_); // default constructor will be called for all slots

			/* copy slots */
			for (size_t i = 0; i < other.table_capacity_; i++)
			{
				// hashcode_key and state will be copied directly
				// the key's and value's will be deep copied, if they exist 
				table_[i] = other.table_[i];
			}

			table_size_ = other.table_size_;
		}

		constexpr hash_map& operator=(hash_map other)
		{
			swap(*this, other);
			return *this;
		}

		constexpr friend void swap(hash_map& lhs, hash_map& rhs) noexcept
		{
			swap(lhs.table_, rhs.table_);
			swap(lhs.table_size_, rhs.table_size_);
			swap(lhs.table_capacity_, rhs.table_capacity_);
		}

		constexpr iterator begin() const
		{
			auto begin_slot = table_;
			auto end_slot = table_ + table_capacity_;

			// find the first occupied slot
			while (begin_slot < end_slot && begin_slot->state_ != occupied)
			{
				++begin_slot;
			}

			return iterator(begin_slot, end_slot);
		}

		constexpr iterator end() const
		{
			auto end_slot = table_ + table_capacity_;
			return iterator(end_slot, end_slot);
		}

		constexpr V& operator [](K key) {
			return find_or_insert(move(key));
		}

		// insert key/value into table
		constexpr table_slot* insert(K key, V value)
		{
			if (table_size_ == table_capacity_ || load_factor() >= 0.6f)
			{
				resize(table_capacity_ ? get_next_table_capacity() : 13);
			}

			auto key_hash_code = crt::get_hash(key);
			return insert(crt::move(key), key_hash_code, crt::move(value), table_, table_capacity_, table_size_);
		}

		// find value by key  returns nullptr if the key does not exist
		constexpr V* find_value(const K& key)
		{
			auto slot = find_slot(key);
			if (slot)
				return &slot->get_value();

			return nullptr;
		}

		constexpr bool key_exists(const K& key)
		{
			return find_slot(key) != nullptr;
		}

		constexpr bool empty() const
		{
			return size() == 0;
		}

		// get current map size
		constexpr size_t size() const
		{
			return table_size_;
		}

		// remove slot
		constexpr void remove(table_slot* s)
		{
			if (s)
			{
				s->remove();
				--table_size_;
			}
		}

		// remove key/value pair if it exists
		constexpr void remove(const K& key)
		{
			auto slot = find_slot(key);
			if (slot)
			{
				remove(slot);
			}
		}

		// returns the value by ref if key exists, otherwise inserts it and returns the value by ref.
		constexpr V& find_or_insert(const K& key)
		{
			auto slot = find_slot(key);
			if (slot)
				return slot->get_value();

			return insert(key, V{})->get_value();
		}

		constexpr float load_factor() const
		{
			return table_capacity_ ? (float)table_size_ / (float)table_capacity_ : 1.f;
		}

		constexpr crt::maybe<V> find_value(const K& key) const
		{
			const auto slot = find_slot(key);
			if (!slot)
				return crt::nothing<V>();
			return crt::just(slot->get_value());
		}

		// find slot by key, returns nullptr if slot does not exist
		constexpr table_slot* find_slot(const K& key) const
		{
			auto key_code = crt::get_hash(key);
			for (size_t probe_index = 0; probe_index < table_capacity_; ++probe_index)
			{
				const auto pos = calculate_position(key_code, probe_index, table_capacity_);

				/* search ended */
				if (table_[pos].state_ == null)
					return nullptr;

				/* ignore deleted values */
				if (table_[pos].state_ == deleted)
					continue;

				const auto& table_key = table_[pos].get_key();

				if (table_[pos].hashcode_key_ == key_code && table_key == key)
				{
					return &table_[pos];
				}

			}

			/* table full, and it doesn't contain the key*/
			return nullptr;
		}

	private:
		/* resize the table. this requires re-hashing of the entire table as the hash functions will change */
		constexpr void resize(size_t new_capacity)
		{
			// this does not invoke the default constructor of keys or values
			// invokes default constructor for all slots -> key/value are not constructed yet, all slots are null.
			table_slot* new_table = new table_slot[new_capacity];
			size_t new_size = 0;

			/* insert all occupied positions in the current table to the new table */
			for (size_t i = 0; i < table_capacity_; i++)
			{
				auto& slot = table_[i];
				if (slot.state_ == occupied)
				{
					// use slot.hashcode_key instead of recalculating it. just calculate the new position.
					K key = crt::move(slot.get_key());
					V value = crt::move(slot.get_value());

					/* insert the key-value pair into the new table */
					insert(crt::move(key), slot.hashcode_key_, crt::move(value), new_table, new_capacity, new_size);
				}
			}

			// invokes the destructor for all of the slots, which then invokes destructors for key/values.
			if (table_)
				delete[] table_;

			table_ = new_table;
			table_size_ = new_size; // assert(table_size_ == new_size)
			table_capacity_ = new_capacity;
		}

		constexpr static table_slot* insert(K key, size_t key_hashcode, V value, table_slot* table, size_t table_capacity, size_t& table_size)
		{
			for (size_t probe_index = 0; probe_index < table_capacity; ++probe_index)
			{
				auto pos = calculate_position(key_hashcode, probe_index, table_capacity);
				if (table[pos].state_ == null || table[pos].state_ == deleted)
				{
					// we construct the object by using the move constructor at the given location
					table[pos].state_ = occupied;
					table[pos].hashcode_key_ = key_hashcode;

					// invoke move constructors for key and value
					table[pos].key_ = crt::move(key);
					table[pos].value_ = crt::move(value);

					++table_size;
					return &table[pos];
				}

				/* the slot is occupied, check if the key is same, if so, overwrite the slot's value */
				const K& table_key = table[pos].get_key();
				if (table[pos].hashcode_key_ == key_hashcode && table_key == key)
				{
					// place the new value. construct_object will automatically call the destructor for the previous object.
					table[pos].value_ = crt::move(value);
					return &table[pos];
				}
			}

			// should not happen
			return nullptr;
		}

		constexpr static size_t gcd(size_t a, size_t b)
		{
			if (a < b)
				swap(a, b);

			for (;;)
			{
				if (b == 0)
					return a;

				const size_t old_a = a;
				const size_t old_b = b;

				a = b;
				b = old_a % old_b;
			}
		}

		constexpr static bool is_prime(size_t number)
		{
			for (size_t i = 2; i < number; ++i)
			{
				if (gcd(number, i) != 1)
					return false;
			}

			return true;
		}

		constexpr static size_t calculate_position(size_t key, size_t probe_index, size_t modulo)
		{
			/* use double hashing to minimize clustering */
			const auto h1 = key % modulo; // initial hash

			const auto h2 = probe_index * (1 + key % (modulo - 1)); // secondary hash

			return (h1 + h2) % modulo;
		}

		// calculate next prime in range (table_size_ * 2 , ... )
		constexpr size_t get_next_table_capacity() const
		{
			auto next_size = table_capacity_ * 2;

			while (!is_prime(++next_size))
			{

			}

			return next_size;
		}


		table_slot* table_;
		size_t table_size_;
		size_t table_capacity_;
	};
}
