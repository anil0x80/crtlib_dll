#pragma once

// todo reorganize to here
namespace crt::in_place
{
	template<typename F, typename Container>
	void drop_if(F f, Container& container)
	{
		for (auto it = container.begin(); it != container.end();)
		{
			if (f(*it))
			{
				it = container.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	template<typename F, typename Container>
	void drop_if_unstable(F f, Container& container)
	{
		for (auto it = container.begin(); it != container.end();)
		{
			if (f(*it))
			{
				it = container.unstable_erase(it);
			}
			else
			{
				++it;
			}
		}
	}

}
