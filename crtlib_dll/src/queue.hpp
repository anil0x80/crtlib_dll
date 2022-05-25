#pragma once

#include "list.hpp"

namespace crt
{
	template <typename T>
	class queue
	{
	public:
		using value_type = T;


		void enqueue(T value)
		{
			list_.push_back(move(value));
		}

		T dequeue()
		{
			return list_.pop_front();
		}

		T& peek()
		{
			return *list_.head_->value;
		}

		const T& peek() const
		{
			return *list_.head_->value;
		}

		size_t size() const
		{
			return list_.size();
		}

		bool empty() const
		{
			return list_.empty();
		}


	private:
		list<T> list_{};
	};
}
