// ReSharper disable CppClangTidyBugproneReservedIdentifier
#include "crt_init.h"
#include "heap_allocator.h"

#pragma section(".CRT$XCA", read)
#pragma data_seg(".CRT$XCA")
_PVFV __xc_a[] = { nullptr };


#pragma section(".CRT$XCZ", read)
#pragma data_seg(".CRT$XCZ")
_PVFV __xc_z[] = { nullptr };

/* merge the .crt section to .rdata section */
#pragma data_seg()
#pragma comment(linker, "/merge:.CRT=.rdata")
//#pragma comment(linker, "/merge:.CRT=.data")

extern "C"
{
	/*
	* static here means keep the variable for the current translation unit only
	* so if any other .cpp file defines them, there won't be any name collisions.
	*/
	static _PVFV* atexitlist = nullptr;
	static unsigned int max_atexit_entries = 0;
	static unsigned int cur_atexit_entries = 0;
	
	void _initterm(_PVFV* pfbegin, _PVFV* pfend)
	{
		while (pfbegin < pfend)
		{
			if (*pfbegin != nullptr)
				(**pfbegin)();  // calling the constructor of a static global causes atexit to be called, registering an exit callback.
			++pfbegin;
		}
	}

	void _init_atexit()
	{
		max_atexit_entries = 128;
		atexitlist = (_PVFV*)crt::alloc(max_atexit_entries * sizeof(_PVFV*));
	}

	int atexit(_PVFV func) // called automatically
	{
		if (cur_atexit_entries < max_atexit_entries)
		{
			atexitlist[cur_atexit_entries++] = func;
			return 0;
		}

		return -1;
	}

	void _doexit()
	{
		if (cur_atexit_entries)
		{
			/* call all of the exit time destructors in reverse order */
			for (auto p_end = atexitlist + cur_atexit_entries - 1; p_end >= atexitlist; --p_end)
			{
				if (*p_end)
					(**p_end)();
			}
		}

		crt::free(atexitlist); // i added this, because i felt like the memory was leaking.
	}
}

