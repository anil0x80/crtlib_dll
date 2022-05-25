#pragma once
#include <type_traits>

namespace crt
{
	template<typename...> struct rebinder;

	template<template<typename...> class Container, typename ... Args>
	struct rebinder<Container<Args...>> {
		template<typename ... UArgs>
		using rebind = Container<UArgs...>;
	};

	template< typename C, typename = void >
	struct has_reserve
		: std::false_type
	{};

	template< typename C >
	struct has_reserve< C, typename std::enable_if<
		std::is_same<
		decltype(std::declval<C>().reserve(std::declval<size_t>())),
		void
		>::value
	>::type >
		: std::true_type
	{};
}
