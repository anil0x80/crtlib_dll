// ReSharper disable CppClangTidyBugproneReservedIdentifier
#pragma once
#include <Windows.h>

/* make sure to use C linkage so our functions can be used by the linker, atexit etc. these names need to be global. */
extern "C"
{
	/* C++ global constructors/destructors pointers */
	typedef void(__cdecl* _PVFV)();
	extern _PVFV __xc_a[], __xc_z[]; // start and end of the ctor section. linker puts the callbacks there.

	/* call these at the startup */
	void _init_atexit();
	void _initterm(_PVFV* pfbegin, _PVFV* pfend);

	/* call this at the exit, calls global destructors */
	void _doexit();


	int _init_args();
	void _term_args();

	extern TCHAR* _argv[];
}
