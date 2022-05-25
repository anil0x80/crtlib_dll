#pragma once
#include "assert.h"
#include <type_traits>

namespace crt
{
	// we either store a T or we don't have it.
	// default constructor doesn't have a value by default
	template<typename T>
	class maybe
	{
	public:
		maybe() = default;

		static maybe nothing()
		{
			return maybe();
		}

		static maybe just(T value)
		{
			return maybe(crt::move(value));
		}

		maybe(T value) : value_(crt::move(value)), has_value_(true)
		{

		}

		friend bool operator== (const maybe& a, const maybe& b)
		{
			if (a.has_value() == b.has_value())
			{
				// both maybe's are nothing
				if (!a.has_value())
					return true;

				// both maybe's are just
				return a.value_ == b.value_;
			}

			// maybe's don't match
			return false;
		}

		bool has_value() const
		{
			return has_value_;
		}

		T& operator* ()
		{
			CRT_ASSERT(has_value_, "maybe dereference without value!");
			return value_;
		}

		T* operator->()
		{
			CRT_ASSERT(has_value_, "maybe dereference without value!");
			return &value_;
		}

		const T& operator* () const
		{
			CRT_ASSERT(has_value_, "maybe dereference without value!");
			return value_;
		}


		const T* operator->() const
		{
			CRT_ASSERT(has_value_, "maybe dereference without value!");
			return &value_;
		}


	private:
		T value_{};
		bool has_value_{ false };
	};

	template <typename U>
	maybe<U> nothing()
	{
		return maybe<U>();
	}

	template <typename U>
	maybe<U> just(const U& value)
	{
		return maybe<U>(value);
	}

	template <typename T>
	bool is_just(const maybe<T>& m)
	{
		return m.has_value();
	}

	template <typename T>
	bool is_nothing(const maybe<T>& m)
	{
		return !m.has_value();
	}

	// ((a -> b), maybe a) -> maybe b
	// apply function f to maybe m if m is just and return just, else return nothing.
	template<typename F, typename T, typename U = std::invoke_result_t<F, T>>
	maybe<U> lift_maybe(F f, const maybe<T>& m)
	{
		if (is_just(m))
		{
			return just<U>(f(*m));
		}

		return nothing<U>();
	}

	// ((a -> maybe b), maybe a) -> maybe b
	// apply f to input if input is just, else return nothing. 
	template<typename F, typename A, typename B = std::invoke_result_t<F, A>>
	auto and_then_maybe(F f, const crt::maybe<A>& in)
	{
		if (is_just(in))
			return f(*in);

		return nothing<B>();
	}

	template<typename T>
	auto just_with_default(const T& def, const crt::maybe<T>& myb)
	{
		if (is_just(myb))
			return *myb;

		return def;
	}
}
