#include "intrinsics_memory.h"


#include "asmlib.h"

extern "C"
{
#ifndef _WIN64
	__declspec(naked) void _ftol2()
	{
		__asm
		{
			fistp qword ptr[esp - 8]
			mov   edx, [esp - 4]
			mov   eax, [esp - 8]
			ret
		}
	}

	__declspec(naked) void _ftol2_sse()
	{
		__asm
		{
			fistp dword ptr[esp - 4]
			mov   eax, [esp - 4]
			ret
		}
	}

	__declspec(naked) void _aullshr()
	{
		__asm
		{
			cmp     cl, 64
			jae     short RETZERO
			;
			; Handle shifts of between 0 and 31 bits
				;
			cmp     cl, 32
				jae     short MORE32
				shrd    eax, edx, cl
				shr     edx, cl
				ret
				;
			; Handle shifts of between 32 and 63 bits
				;
		MORE32:
			mov     eax, edx
				xor edx, edx
				and cl, 31
				shr     eax, cl
				ret
				;
			; return 0 in edx : eax
				;
		RETZERO:
			xor eax, eax
				xor edx, edx
				ret
		}
	}
#endif
}

#pragma function(memcpy) // tell compiler to not inline this. by doing this, we prevent errors about re-defined intrinsics.
void* memcpy(void* dst, void const* src, size_t size)
{
	return A_memcpy(dst, src, size);
}

#pragma function(memset)
void* __cdecl memset(void* dst, int c, size_t size)
{
	return A_memset(dst, c, size);
}

#pragma function(memcmp)
int memcmp(const void* m1, const void* m2, size_t size)
{
	return A_memcmp(m1, m2, size);
}

#pragma function(memmove)
void* memmove(void* dest, const void* src, size_t count)
{
	return A_memmove(dest, src, count);
}
