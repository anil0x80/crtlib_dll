#pragma once

#include "list.hpp"


namespace crt
{
	template <typename T>
	class stack
	{
	public:
		using value_type = T;

		void push(T value)
		{
			list_.push_front(move(value));
		}

		T pop()
		{
			return list_.pop_front();
		}

		// discard elements after num. for example limit(2) means items after first two items will be deleted.
		void limit(size_t num)
		{
			while(size() > num)
			{
				list_.pop_back();
			}
		}

		T& peek()
		{
			return *list_.begin();
		}

		const T& peek() const
		{
			return *list_.cbegin();
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
		list<T> list_;
	};
}
