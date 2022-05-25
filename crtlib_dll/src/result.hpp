#pragma once
#include <type_traits>

#include "assert.h"

namespace crt
{
	// holds result or error type
	template <typename R, typename E>
	class result
	{
	public:
		using ok_type = R;
		using error_type = E;

		static result ok(R r)
		{
			return result<R, E>(move(r));
		}

		static result error(E e)
		{
			return result<R, E>(move(e));
		}

		result(R r) : result_(move(r)), has_result_(true)
		{

		}

		result(E e) : error_(move(e)), has_result_(false)
		{

		}

		bool has_result() const
		{
			return has_result_;
		}

		R& get_ok()
		{
			CRT_ASSERT(has_result_, "get_result does not have a result!");
			return result_;
		}

		const R& get_ok() const
		{
			CRT_ASSERT(has_result_, "get_result does not have a result!");
			return result_;
		}


		E& get_error()
		{
			CRT_ASSERT(!has_result_, "get_error does not have an error!");
			return error_;
		}

		const E& get_error() const
		{
			CRT_ASSERT(!has_result_, "get_error does not have an error!");
			return error_;
		}

		R& operator* ()
		{
			CRT_ASSERT(has_result_, "result dereference without value!");
			return result_;
		}

		R* operator->()
		{
			CRT_ASSERT(has_result_, "result dereference without value!");
			return &result_;
		}

		const R& operator* () const
		{
			CRT_ASSERT(has_result_, "result dereference without value!");
			return result_;
		}


		const R* operator->() const
		{
			CRT_ASSERT(has_result_, "result dereference without value!");
			return &result_;
		}




	private:
		R result_{};
		E error_{};

		bool has_result_{};
	};

	template <typename R, typename E>
	result<R, E> error(const E& e)
	{
		return result<R, E>(e);
	}

	template <typename R, typename E>
	result<R, E> ok(R r)
	{
		return result<R, E>(move(r));
	}

	template <typename R, typename E>
	bool is_ok(const result<R, E>& result)
	{
		return result.has_result();
	}

	template <typename R, typename E>
	bool is_error(const result<R, E>& result)
	{
		return !result.has_result();
	}

	// ((a -> b), Result a) -> Result b
	// apply function f to result r if r is ok and return ok, else carry the error.
	template<typename F, typename In, typename E, typename Out = std::invoke_result_t<F, In>>
	result<Out, E> lift_result(F f, result<In, E> r)
	{
		if (is_ok(r))
		{
			return ok<Out, E>(f(crt::move(r.get_ok())));
		}

		return error<Out, E>(r.get_error());
	}

	// ((a -> b), Result a) -> Result b
	// apply function f to result r if r is ok and return ok, else carry the error.
	template<typename F, typename In, typename E, typename Out = std::invoke_result_t<F, In>>
	result<Out, E> lift_result_move(F f, result<In, E>& r)
	{
		if (is_ok(r))
		{
			return ok<Out, E>(f(move(r.get_ok())));
		}

		return error<Out, E>(r.get_error());
	}

	template <typename Ok, typename Error, typename F, typename Out = typename std::invoke_result_t<F, Ok>::ok_type>
	auto and_then_result(F f, const result<Ok, Error>& r)
	{
		if (is_ok(r))
		{
			return f(r.get_ok());
		}

		return error<Out, Error>(r.get_error());
	}

	template <typename Ok, typename Error, typename F, typename Out = typename std::invoke_result_t<F, Ok>::ok_type>
	auto and_then_result_move(F f, result<Ok, Error> r)
	{
		if (is_ok(r))
		{
			return f(move(r.get_ok()));
		}

		return error<Out, Error>(r.get_error());
	}

	template <typename Ok, typename Error>
	Ok ok_with_default(const Ok& default_value, const result<Ok, Error>& result)
	{
		if (is_ok(result))
		{
			return result.get_ok();
		}

		return default_value;
	}
}

