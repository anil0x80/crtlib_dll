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


extern int _tmain(int, TCHAR** const, TCHAR** const);


EXTERN_C void mainCRTStartup()
{
    crt::heap_initialize();
    int argc = _init_args();
    _init_atexit();
    _initterm(__xc_a, __xc_z);

    int ret = _tmain(argc, _argv, 0);

    _doexit();
    _term_args();
    ExitProcess(ret);
}