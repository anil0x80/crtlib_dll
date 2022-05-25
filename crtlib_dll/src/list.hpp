#pragma once
#include "smart_ptr.hpp"

namespace crt
{
	template <typename T>
	class list
	{
		struct node
		{
			T value;
			node* next;
			node* prev;
		};

		using value_type = T;

		struct const_iterator
		{
			constexpr static auto tag()
			{
				return forward_iterator_tag{};
			}

			using pointer = const T*;
			using reference = const T&;
			using value_type = T;

			const_iterator(node* p) : ptr_(p) {}

			reference operator*() const { return ptr_->value; }
			pointer operator->() { return &ptr_->value; }

			const_iterator& operator++() { ptr_ = ptr_->next; return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.ptr_ == b.ptr_; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.ptr_ != b.ptr_; }

		private:
			node* ptr_;
		};

		struct iterator
		{
			constexpr static auto tag()
			{
				return forward_iterator_tag{};
			}

			friend class list;

			using pointer = T*;
			using reference = T&;
			using value_type = T;

			iterator(node* p) : ptr_(p){}

			reference operator*() const { return ptr_->value; }
			pointer operator->() { return &ptr_->value; }

			iterator& operator++() { ptr_ = ptr_->next; return *this; }
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const iterator& a, const iterator& b) { return a.ptr_ == b.ptr_; };
			friend bool operator!= (const iterator& a, const iterator& b) { return a.ptr_ != b.ptr_; }

		private:
			node* ptr_;
		};

	public:
		// resource management
		list() = default;

		list(std::initializer_list<T> args)
		{
			for (const T& elem : args)
			{
				push_back(elem);
			}
		}

		list(const list& other) // copy constructor
		{
			auto node = other.head_;
			while (node != nullptr)
			{
				auto next = node->next;
				push_back(node->value); // invoke copy constructor for T
				node = next;
			}
		}

		list(list&& other) // move constructor
		{
			swap(head_, other.head_);
			swap(tail_, other.tail_);
			swap(size_, other.size_);
		}

		list& operator=(list&&other) // move assignment
		{
			destroy();
			swap(head_, other.head_);
			swap(tail_, other.tail_);
			swap(size_, other.size_);

			return *this;
		}

		list& operator=(const list& other) // copy assignment
		{
			destroy();

			auto node = other.head_;
			while (node != nullptr)
			{
				auto next = node->next;
				push_back(node->value); // invoke copy constructor for T
				node = next;
			}
			return *this;;
		}

		~list()
		{
			destroy();
		}

		void destroy()
		{
			auto node = head_;
			while (node != nullptr)
			{
				auto next = node->next;
				delete node;
				node = next;
			}

			head_ = nullptr;
			tail_ = nullptr;
			size_ = 0;
		}

		// iterators
		iterator begin() { return iterator(head_); }
		iterator end()	 { return iterator(nullptr); }

		const_iterator begin() const { return const_iterator(head_); }
		const_iterator end() const { return const_iterator(nullptr); }
		
		// insert value at the tail of the list
		void push_back(T value)
		{
			insert(end(), move(value));
		}

		void push_front(T value)
		{
			insert(begin(), move(value));
		}

		void insert(iterator pos, T value)
		{
			++size_;
			node* target = pos.ptr_;

			node* prev{};
			node* next{};
			if (!target)
			{
				// insert to end
				prev = tail_;
			}
			else
			{
				prev = target->prev;
				next = target->next;
			}

			if (target == head_)
			{
				next = head_;
			}


			auto new_node = new node{ move(value), next, prev };

			if (prev)
				prev->next = new_node;
			if (next)
				next->prev = new_node;


			if (empty())
			{
				head_ = new_node;
				tail_ = head_;
			}

			if (!target)
			{
				// update tail if inserting to end
				tail_ = new_node;
			}

			if (target == head_)
			{
				// update head if inserting to head
				head_ = new_node;
			}
		}

		void erase(iterator it)
		{
			--size_;
			node* target = it.ptr_;

			if (!target->prev && !target->next)
			{
				// list had only 1 element, and now we delete it.
				delete target;
				head_ = nullptr;
				tail_ = nullptr;
				return;
			}

			if (!target->prev)
			{
				// delete first element of the list
				target->next->prev = nullptr; // unlink the next element
				head_ = target->next; // fix head
				delete target;
				return;
			}

			if (!target->next)
			{
				// delete last element of the list
				target->prev->next = nullptr; // unlink the previous element
				tail_ = target->prev; // fix the tail
				delete target;
				return;
			}

			// delete middle element of the list
			target->prev->next = target->next;
			target->next->prev = target->prev;
			delete target;
		}

		T pop(iterator it)
		{
			auto val = move(*it);
			erase(it);
			return val;
		}

		T pop_back()
		{
			return pop(tail_);
		}

		T pop_front()
		{
			return pop(head_);
		}

		bool empty() const
		{
			return head_ == nullptr;
		}

		size_t size() const
		{
			return size_;
		}

	private:
		node* head_{};
		node* tail_{};
		size_t size_{};
	};
}