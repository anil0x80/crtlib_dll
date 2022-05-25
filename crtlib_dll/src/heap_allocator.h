#pragma once
#include <cstdint>

constexpr size_t default_alignment = 8;

namespace crt
{
	/* should be called once at the beginning of the program, usually crt_init takes care of this.*/
	void heap_initialize();

	void* alloc(size_t size, size_t alignment = default_alignment);
	void* realloc(void* ptr, size_t size, size_t alignment = default_alignment);
	void free(void* ptr, size_t alignment = default_alignment);
}

/*
 *The versions (1-4) are implicitly declared in each translation unit even if the <new> header is not included.
 *Versions (1-8) are replaceable: a user-provided non-member function with the same signature defined anywhere in the program,
 *in any source file, replaces the default version.
 *Its declaration does not need to be visible.
 * 
 */

// these need to be declared inline otherwise a fucking linker error occurs.
// but if you try to declare any other as inline it will tel you its impossible.
// todo switch inline -> extern if linker error occurs
inline void* operator new  (size_t count, void* ptr) noexcept;
inline void* operator new[](size_t count, void* ptr) noexcept;
