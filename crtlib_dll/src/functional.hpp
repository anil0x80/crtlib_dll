#pragma once
#include "result.hpp"

namespace crt
{
	// ((a,a)->b, a) -> (a -> b)
	template<typename F, typename T>
	auto bind_1st_of_2(F f, T bind)
	{
		return [f, bind](auto arg)
		{
			return f(bind, crt::move(arg));
		};
	}


	template<typename F, typename T>
	auto bind_2nd_of_2(F f, T bind)
	{
		return [f, bind](auto arg)
		{
			return f(crt::move(arg), bind);
		};
	}

	// compose: ((a -> b), (b -> c)) -> (a -> c)
	// todo implement for arbitrary number of functions
	// useful when doing more than 1 transforms, combining them into 1 loop.
	template<typename F1, typename F2>
	auto compose(F1 f1, F2 f2)
	{
		return [f1, f2](const auto& a)
		{
			return f2(f1(a));
		};
	}

	template<typename F>
	constexpr auto logical_not(F f)
	{
		return [f](auto... params) {return !f(params...); };
	}

	template<typename F1, typename F2>
	auto logical_or(F1 f1, F2 f2)
	{
		return [f1, f2](const auto& x) {return f1(x) || f2(x); };
	}

	template<typename F>
	auto unwrapper_maybe(F f)
	{
		return [f](auto... params)
		{
			const auto result = f(params...);
			CRT_ASSERT(is_just(result), "Unwrapped nothing!");
			return result.get_value();
		};
	}

	template<typename T, typename F>
	auto cast_result(F f)
	{
		return [f](auto... params) {return static_cast<T>(f(params...)); };
	}

	template<typename T>
	auto is_not_equal_bind(T val)
	{
		return [val](const auto& x) {return x != val; };
	}

	template<typename T>
	auto is_equal_bind(T val)
	{
		return [val](const auto& x) {return x == val; };
	}

	constexpr auto add = [](const auto& a, const auto& b) {return a + b; };
	constexpr auto multiply = [](const auto& a, const auto& b) {return a * b; };
	constexpr auto square = [](const auto& x) {return x * x; };
	constexpr auto identity = [](const auto& x) {return x; };
	constexpr auto is_equal = [](const auto& x, const auto& y) {return x == y; };
	constexpr auto is_not_equal = [](const auto& x, const auto& y) {return x != y; };


	// parse_arguments -> result<params>
	// params -> read_file -> result<params_file>
	// params_file -> create_capstone_file -> result<capstone_file>
	// capstone_file -> dump_function -> result<Instructions>

	// chain(parse_arguments(argc,argv), read_file, create_capstone_file, dump_function);
	//        first result				--- intermediate ---				last result



	template <typename Ok, typename Error>
	auto chain_result(crt::result<Ok, Error> input)
	{
		return crt::move(input);
	}

	template <typename Ok, typename Error, typename F, typename... Functions>
	auto chain_result(crt::result<Ok, Error> input, F func, Functions... functions)
	{
		using t_result = std::invoke_result_t<F, Ok>;

		if (is_ok(input))
			return chain_result<t_result::ok_type, t_result::error_type>(func(crt::move(input.get_ok())), functions...);


		return chain_error<t_result::ok_type, t_result::error_type>(t_result::error(input.get_error()), functions...);
	}


	template <typename Ok, typename Error>
	auto chain_error(crt::result<Ok, Error> input)
	{
		return crt::move(input);
	}

	template <typename Ok, typename Error, typename F, typename... Functions >
	auto chain_error(crt::result<Ok, Error> input, F func, Functions... functions)
	{
		using t_result = std::invoke_result_t<F, Ok>;
		return chain_error<t_result::ok_type, t_result::error_type>(t_result::error(input.get_error()), functions...);
	}


}