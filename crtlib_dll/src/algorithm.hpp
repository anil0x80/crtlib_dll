#pragma once
#include "my_vector.hpp"
#include <type_traits>
#include "algorithm_sort.hpp"
#include "in_place.hpp"
#include "functional.hpp"
#include "iterator.hpp"
#include "type.hpp"
#include "maybe.hpp"

namespace crt
{
	template <typename IteratorType, typename ValueType>
	auto  find_first_not_of(IteratorType begin, IteratorType end, const ValueType& value)
	{
		for (auto it = begin; it != end; ++it)
		{
			if (*it != value)
				return it;
		}

		return end;
	}

	template <typename IteratorType, typename ValueType>
	auto find_first_of(IteratorType begin, IteratorType end, const ValueType& value)
	{
		for (auto it = begin; it != end; ++it)
		{
			if (*it == value)
				return it;
		}

		return end;
	}

	template <typename IteratorType, typename ValueType>
	auto find_last_of(IteratorType begin, IteratorType end, const ValueType& value)
	{
		for (auto it = end - 1; it != begin - 1; --it)
		{
			if (*it == value)
				return it;
		}

		return IteratorType{};
	}

	template <typename IteratorType>
	void reverse(IteratorType begin, IteratorType end)
	{
		auto p_start = begin;
		auto p_last = end - 1;

		while (p_last > p_start)
		{
			crt::swap(*p_start, *p_last);
			++p_start;
			--p_last;
		}
	}

	template <typename IteratorType, typename Predicate>
	auto find_if(IteratorType begin, IteratorType end, Predicate p)
	{
		for (auto it = begin; it != end; ++it)
		{
			if (p(*it))
				return it;
		}

		return end;
	}

	template <typename IteratorType>
	auto average(IteratorType begin, IteratorType end)
	{
		if (begin != end)
		{
			auto result = *begin;

			size_t count = 1;
			++begin;
			while (begin != end)
			{
				result += *begin;
				++begin;
				++count;
			}

			return result / count;
		}

		return IteratorType::value_type();
	}

	// get all possible unique combinations of size N from list
	template<typename T>
	crt::vector<crt::vector<T>> combinations_internal(T* list, size_t list_start,
		size_t list_size, size_t n, crt::vector<T> prev_array)
	{
		if (prev_array.size() == n)
		{
			crt::vector<crt::vector<T>> res;
			res.push_back(crt::move(prev_array));
			return res;
		}

		crt::vector<crt::vector<T>> combs;
		for (size_t i = list_start; i < list_size; ++i)
		{
			auto copy = prev_array;
			copy.push_back(list[i]);

			combs.insert(combs.end(), combinations_internal(list, i + 1, list_size, n, crt::move(copy)));
		}

		return combs;
	}

	// return all unique combinations of size N made from elements of vec
	template<typename T>
	crt::vector<crt::vector<T>> combinations(const crt::vector<T>& vec, size_t n)
	{
		return combinations_internal(vec.data(), 0, vec.size(), n, {});
	}

	template<typename F, typename Container>
	auto keep_if(F function, const Container& values)
	{
		Container result{};

		for (const auto& val : values)
		{
			if (function(val))
				result.push_back(val);
		}

		return result;
	}

	template<typename F, typename Container>
	auto drop_if(F f, const Container& values)
	{
		return keep_if(crt::logical_not(f), values);
	}

	// use this for working with inner sequences like vec<vec<int>> with transform.
	// (a -> Bool) -> [a] -> [a]
	template<typename F>
	auto keep_if_c(F f)
	{
		return [f](const auto& container)
		{
			std::remove_const_t<std::remove_reference_t<decltype(container)>> result{};
			for (const auto& val : container)
				if (f(val))
					result.push_back(val);
			return result;
		};
	}


	template<typename F, typename Container,
		typename T = typename Container::value_type, typename OutType = std::invoke_result_t<F, T>,
		typename ContainerOut = typename rebinder<Container>::template rebind<OutType>>
		auto transform(F function, const Container& values)
	{
		ContainerOut result{};

		if constexpr (has_reserve<ContainerOut>::value)
			result.reserve(values.size());

		for (auto& val : values)
		{
			result.push_back(function(val));
		}

		return result;
	}

	// ([a], it, it) -> [a]
	// create slice of the container in between two given iterators.
	// [5,10,15,20], begin + 2, end  -> [15, 20]
	// O(n)
	template<typename ContainerIn, typename IteratorType, typename T = typename ContainerIn::value_type>
	auto slice(const ContainerIn& container, const IteratorType& begin, const IteratorType& end)
	{
		const auto size = crt::distance(begin, end);

		ContainerIn result{};
		result.reserve(size);

		for (auto it = begin; it != end; ++it)
		{
			result.push_back(*it);
		}

		return result;
	}

	// slice by index that is safe, if the indexes are bad, it will return empty container.
	template<typename ContainerIn, typename T = typename ContainerIn::value_type>
	auto slice_idx(const ContainerIn& container, const size_t idx_begin, const size_t idx_end)
	{
		const auto size = container.size();
		if (!size || idx_begin > size - 1 || idx_end > size || idx_begin > idx_end)
			return ContainerIn{};

		const auto begin = container.begin();
		return slice(container, crt::advance(begin, idx_begin), crt::advance(begin, idx_end));
	}

	// O(n)
	template<typename ContainerIn, typename T = typename ContainerIn::value_type,
		typename ContainerOut = typename crt::vector<ContainerIn>>
		ContainerOut split(const T& separator, const ContainerIn& in)
	{
		using iterator_in = decltype(std::declval<ContainerIn>().begin());

		ContainerOut parts{}; // vec<str<a>>
		parts.reserve(in.size()); // worst case O(n) elements will be needed.

		auto left = in.begin();
		for (auto right = in.begin(); right != in.end(); ++right)
		{
			if (*right == separator && crt::distance(left, right) > 0) // bug on list iterator
			{
				parts.emplace_back(crt::slice(in, left, right));
				left = crt::advance(right, 1);//fucking bug  on list iterator
			}
		}

		if (crt::distance(left, in.end()) > 0)
		{
			parts.emplace_back(crt::slice(in, left, in.end()));
		}

		return parts;
	}

	template<typename ContainerIn, typename F,
		typename ContainerOut = typename crt::vector<ContainerIn>>
		ContainerOut split_if(F is_seperator, const ContainerIn& in)
	{
		using iterator_in = decltype(std::declval<ContainerIn>().begin());

		ContainerOut parts{}; // vec<str<a>>
		parts.reserve(in.size()); // worst case O(n) elements will be needed.

		auto left = in.begin();
		for (auto right = in.begin(); right != in.end(); ++right)
		{
			if (is_seperator(*right) && crt::distance(left, right) > 0) // bug on list iterator
			{
				parts.emplace_back(crt::slice(in, left, right));
				left = crt::advance(right, 1);//fucking bug  on list iterator
			}
		}

		if (crt::distance(left, in.end()) > 0)
		{
			parts.emplace_back(crt::slice(in, left, in.end()));
		}

		return parts;
	}

	template<typename Container, typename T = typename Container::value_type, typename F>
	Container replace_if(F should_replace, const T& new_value, Container&& in)
	{
		Container new_container = crt::move(in);
		for(auto& val : new_container)
		{
			if (should_replace(val))
				val = new_value;
		}
		return new_container;
	}

	template<typename F, typename ContainerType, typename T = typename ContainerType::value_type>
	auto reduce(F function, T initial_value, const ContainerType& container)
	{
		T aggregate = crt::move(initial_value);
		for (const auto& val : container)
		{
			aggregate = function(aggregate, val);
		}

		return aggregate;
	}

	// crashes if sequence is empty
	template<typename F, typename ContainerType, typename T = typename ContainerType::value_type>
	auto maximum_on(F scalar, const ContainerType& container)
	{
		T maximum = *container.begin();

		for (const auto& val : container)
		{
			if (scalar(val) > scalar(maximum))
				maximum = val;
		}

		return maximum;
	}

	// crashes if sequence is empty
	template<typename F, typename ContainerType, typename T = typename ContainerType::value_type>
	auto minimum_on(F scalar, const ContainerType& container)
	{
		T minimum = *container.begin();

		for (const auto& val : container)
		{
			if (scalar(val) < scalar(minimum))
				minimum = val;
		}

		return minimum;
	}

	template<typename ContainerType, typename T = typename ContainerType::value_type >
	auto sum(const ContainerType& container)
	{
		return reduce(crt::add, 0, container);
	}

	// (a -> b), [a] -> b
	// convert type to integral type using scalar f and sum all elements
	template<typename F, typename ContainerType,
		typename T = typename ContainerType::value_type, typename V = std::invoke_result_t<F, T>>
		auto sum_on(F f, const ContainerType& container)
	{
		return reduce([f](auto v, const T& v1) {return v + f(v1); }, V{}, container);
	}

	template<typename ContainerType>
	auto insert_end(ContainerType& in, const ContainerType& to_insert)
	{
		in.insert(in.end(), to_insert.begin(), to_insert.end());
	}

	template<typename ContainerType, typename T = typename ContainerType::value_type >
	auto product(const ContainerType& container)
	{
		return reduce(crt::multiply, 1, container);
	}

	// join("-", ["a", "b", "c"]) == "a-b-c"
	template <typename Container, typename X = typename Container::value_type>
	X join(const X& separator, const Container& container)
	{
		if (container.empty())
			return X{};

		// allocate space for inner sequences + seperators 
		const size_t total_size = crt::sum_on([](const X& c) {return c.size(); }, container) + container.size();

		X value{};
		value.reserve(total_size);

		auto it = container.begin();
		for (size_t i = 0; i < container.size() - 1; ++i)
		{
			insert_end(value, *it);
			insert_end(value, separator);
			++it;
		}

		if (!container.empty())
			insert_end(value, container.back());

		return value;
	}

	// O(n)
	// checks if all elements of the given container satisfies predicate F
	template<typename F, typename ContainerIn>
	bool all_by(F f, const ContainerIn& container)
	{
		for (const auto& val : container)
		{
			if (!f(val))
				return false;
		}

		return true;
	}

	// return the first value that is accepted by the given predicate
	template<typename F, typename ContainerIn, typename T = typename ContainerIn::value_type>
	crt::maybe<T> find_first_by(F f, const ContainerIn& container)
	{
		for (const auto& value : container)
		{
			if (f(value))
				return just(value);
		}

		return nothing<T>();
	}

	template<typename Container>
	bool is_valid_index(size_t index, const Container& container)
	{
		return index < container.size();
	}

	// return the first value that is accepted by the given predicate
	template<typename F, typename ContainerIn>
	crt::maybe<size_t> find_first_idx_by(F f, const ContainerIn& container)
	{
		size_t i = 0;
		for (const auto& value : container)
		{
			if (f(value))
				return just(i);
			++i;
		}

		return nothing<size_t>();
	}

	// returns a vector of all indexes that are matchedby the given predicate
	template<typename F, typename ContainerIn>
	crt::vector<size_t> find_all_idx_by(F f, const ContainerIn& container)
	{
		crt::vector<size_t> matches;

		size_t i = 0;
		for(const auto& value : container)
		{
			if (f(value))
				matches.push_back(i);

			++i;
		}

		return matches;
	}

	// return the first value that is accepted by the given predicate
	template<typename F, typename ContainerIn>
	crt::maybe<size_t> find_last_idx_by(F f, const ContainerIn& container)
	{
		for(int i = container.size() - 1; i >= 0; --i)
		{
			if (f(container[i]))
				return just(static_cast<size_t>(i));
		}

		return nothing<size_t>();
	}

	// return first index that is equal to key
	template<typename ContainerIn, typename T = typename ContainerIn::value_type>
	crt::maybe<size_t> find_first_idx(const T& key, const ContainerIn& container)
	{
		size_t i = 0;
		for (const auto& value : container)
		{
			if (value == key)
				return just(i);

			++i;
		}

		return nothing<size_t>();
	}

	// return last index that is equal to key
	template<typename ContainerIn, typename T = typename ContainerIn::value_type>
	crt::maybe<size_t> find_last_idx(const T& key, const ContainerIn& container)
	{
		if(container.empty())
			return nothing<size_t>();

		auto it = crt::advance(container.begin(), container.size() - 1);
		for(int i = static_cast<int>(container.size()) - 1; i >= 0; --i)
		{
			if (*it-- == key)
				return just<size_t>(i);
		}

		return nothing<size_t>();
	}

	// scale values of container by F and search for key 
	template<typename F, typename Container, typename U, typename T = typename Container::value_type>
	crt::maybe<size_t> find_first_idx_on(F scalar, const U& key, const Container& container)
	{
		size_t i = 0;
		for (const auto& value : container)
		{
			if (scalar(value) == key)
				return just(i);
			++i;
		}

		return nothing<size_t>();
	}

	// return all distinct elements in the container.
	template<typename Container, typename T = typename Container::value_type>
	auto distinct(const Container& container)
	{
		Container result{};
		result.reserve(container.size());

		for (const auto& value : container)
		{
			if (is_nothing(find_first_idx(value, result)))
				result.push_back(value);
		}

		return result;
	}


	// make elements of the container unique with respect to their values by predicate
	template<typename F, typename Container,
		typename T = typename Container::value_type, typename U = std::invoke_result_t<F, T>>
		auto distinct_on(F f, const Container& container)
	{
		crt::vector<U> processed;
		processed.reserve(container.size());

		Container result{};
		result.reserve(container.size());

		for (const auto& value : container)
		{
			const auto scalar = f(value);
			if (is_nothing(find_first_idx(scalar, processed)))
			{
				result.push_back(value);
				processed.push_back(scalar);
			}
		}

		return result;
	}

	template<typename F, typename Container,
		typename T = typename Container::value_type, typename OutType = std::invoke_result_t<F, T>,
		typename ContainerOut = typename rebinder<Container>::template rebind<OutType>>
		auto transform_distinct(F function, const Container& values)
	{
		ContainerOut result{};

		if constexpr (has_reserve<ContainerOut>::value)
			result.reserve(values.size());

		for (const auto& val : values)
		{
			const auto transformed = function(val);

			if (is_nothing(find_first_idx(transformed, result)))
				result.push_back(function(val));
		}

		return result;
	}

	// checks if one of the elements in the container is equal to given element
	//template<typename Container, typename T = typename Container::value_type>
	//bool is_elem_of(const T& key, const Container& container)
	//{
	//	return is_just(find_first_idx(key, container));
	//}

	constexpr auto is_elem_of = [](const auto& key, const auto& container)
	{
		return is_just(find_first_idx(key, container));
	};

	constexpr auto is_not_elem_of = logical_not(is_elem_of);

	// checks if one of the elements in the container fulfills the predicate
	template<typename F, typename Container>
	bool is_elem_of_by(F f, const Container& container)
	{
		for (const auto& value : container)
		{
			if (f(value))
				return true;
		}

		return false;
	}

	// checks if one of the elements of container after applying scalar predicate F is equal to key
	// equivalent to find(key, transform(scalar, container)) but without creating the new container.
	template<typename F, typename U, typename Container>
	bool is_elem_of_on(F scalar, const U& key, const Container& container)
	{
		for (const auto& value : container)
		{
			if (scalar(value) == key)
				return true;
		}

		return false;
	}

	template<typename Container>
	bool any(const Container& container)
	{
		for (const bool& value : container)
		{
			if (value)
			{
				return true;
			}
		}

		return false;
	}

	template<typename Container>
	bool none(const Container& container)
	{
		for (const bool& value : container)
		{
			if (value)
			{
				return false;
			}
		}

		return true;
	}

	template<typename Container>
	bool all(const Container& container)
	{
		for (const bool& value : container)
		{
			if (!value)
			{
				return false;
			}
		}

		return true;
	}

	template<typename Container>
	size_t count(const typename Container::value_type& value, const Container& container)
	{
		size_t result = 0;
		for (const auto& v : container)
		{
			if (v == value)
				++result;
		}
		return result;
	}

	template<typename F, typename ContainerIn>
	size_t count_if(F f, const ContainerIn& container)
	{
		size_t i = 0;
		for (const auto& value : container)
		{
			if (f(value))
				++i;
		}

		return i;
	}
}
