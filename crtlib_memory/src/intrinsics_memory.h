#pragma once

extern "C"
{
	void* __cdecl memcpy(void* dst, void const* src, size_t size);
#pragma intrinsic(memcpy) // tell compiler that it can use an inlined intrinsic instead of this call.

	void* __cdecl memset(void* dst, int c, size_t size);
#pragma intrinsic(memset)

	int  __cdecl memcmp(const void* m1, const void* m2, size_t size);
#pragma intrinsic(memcmp)

	void* __cdecl memmove(void* dest, const void* src, size_t count);
#pragma intrinsic(memmove)
}

