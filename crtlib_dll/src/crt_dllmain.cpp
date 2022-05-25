// ReSharper disable CppClangTidyBugproneReservedIdentifier
#include <Windows.h>
#include "crt_init.h"
#include "heap_allocator.h"

/* workaround for some nasty compile error, caused by the lack of CRT */
#ifdef __cplusplus
extern "C" {
#endif
	int _fltused = 0; // it should be a single underscore since the double one is the mangled name

#ifdef __cplusplus
}
#endif

BOOL WINAPI DllMain(HANDLE, DWORD, LPVOID);

EXTERN_C BOOL WINAPI _DllMainCRTStartup(const HANDLE hInst, const DWORD reason, const LPVOID imp)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		crt::heap_initialize();
		_init_atexit();
		_initterm(__xc_a, __xc_z);
	}

	/* call user DllMain */
	BOOL ret = DllMain(hInst, reason, imp);

	if (reason == DLL_PROCESS_DETACH)
	{
		_doexit();
	}

	return ret;
}

static bool called = false;
extern "C" __declspec(dllexport) void __cdecl cdecl_startup_handler()
{
	if (called)
		return;
	called = true;

	_DllMainCRTStartup(nullptr, DLL_PROCESS_ATTACH, nullptr);
}

