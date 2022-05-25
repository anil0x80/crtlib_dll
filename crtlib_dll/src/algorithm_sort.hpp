#pragma once

namespace crt
{
	template <typename IteratorType, typename Predicate>
	auto insertion_sort(IteratorType begin, IteratorType end, Predicate p)
	{
		const int n = static_cast<int>(end - begin);

		for (int i = 1; i < n; i++)
		{
			int j = i - 1;

			while (j >= 0 && p(*(begin + j), *(begin + j + 1)))
			{
				crt::swap(*(begin + j + 1), *(begin + j));
				j = j - 1;
			}
		}
	}

	template <typename IteratorType, typename Predicate>
	auto quicksort_partition(IteratorType begin, IteratorType end, Predicate p)
	{
		auto pivot = end - 1; // pivot is the last element

		auto i = begin - 1; // this points to the last element of the left partition
		for (auto j = begin; j != end - 1; ++j)
		{
			// is j less than pivot?
			if (!p(*j, *pivot))
			{
				++i;
				crt::swap(*j, *i);
			}
		}

		// put the pivot in its place
		crt::swap(*(i + 1), *pivot);

		// return a iterator to the pivot
		return i + 1;
	}

	template <typename IteratorType, typename Predicate>
	void quicksort(IteratorType begin, IteratorType end, Predicate p)
	{
		const int n = static_cast<int>(end - begin);
		if (n < 10)
		{
			// fall back to insertion sort for small range of data
			return insertion_sort(begin, end, p);
		}

		if (begin != end)
		{
			auto pivot = quicksort_partition(begin, end, p);

			quicksort(begin, pivot, p);
			quicksort(pivot + 1, end, p);
		}
	}

	template <typename IteratorType, typename Predicate>
	auto sort(IteratorType begin, IteratorType end, Predicate p)
	{
		quicksort(begin, end, p);
	}

	template <typename Container, typename F, typename T = typename Container::value_type>
	auto sort_increasing_on(F scalar, Container& container)
	{
		sort(container.begin(), container.end(), [scalar](const T& lhs, const T& rhs)
			{
				return scalar(lhs) > scalar(rhs);
			});
	}

	template <typename Container, typename F, typename T = typename Container::value_type>
	auto sort_decreasing_on(F scalar, Container& container)
	{
		sort(container.begin(), container.end(), [scalar](const T& lhs, const T& rhs)
		{
			return scalar(lhs) < scalar(rhs);
		});
	}
}