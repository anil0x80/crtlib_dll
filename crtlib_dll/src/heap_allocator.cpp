#include "heap_allocator.h"


#include <Windows.h>

namespace std
{
	enum class align_val_t : size_t {};
}

typedef struct _ANSI_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR  Buffer;
} ANSI_STRING;

#ifdef _WIN64
extern "C"
{
	//NTSYSCALLAPI
		//ULONG
		//NTAPI
		//RtlGetProcessHeaps(ULONG number_of_heaps, PVOID* heap_array);

	NTSYSCALLAPI
		PVOID
		NTAPI
		RtlAllocateHeap(PVOID heap, ULONG flags, ULONG size);

	NTSYSCALLAPI
		BOOLEAN
		NTAPI
		RtlFreeHeap(PVOID heap, ULONG flags, PVOID memory);


	NTSYSCALLAPI
		PVOID
		NTAPI
		RtlReAllocateHeap(PVOID heap, ULONG flags, PVOID memory, ULONG size);

}
#endif


/* current allocator state */
static struct
{
	HANDLE h_heap;

} state;

static void* get_ptr_from_mem(void* mem, size_t size)
{
	*(size_t*)mem = size;

	if ((uintptr_t)mem % 16 == 0)
	{
		// 16 byte aligned
		*(uint32_t*)((uint8_t*)mem + 12) = 0xDDDDDDDD; // mark as 16 byte aligned
		return (uint8_t*)mem + 16;
	}
	else
	{
		*(uint32_t*)((uint8_t*)mem + 4) = 0xCCCCCCCC;
		return (uint8_t*)mem + 8;
	}
}

static void* get_mem_from_ptr(void* ptr, size_t* size = nullptr)
{
	const auto tag = *(uint32_t*)((uint8_t*)ptr - 4);
	const auto mem = tag == 0xCCCCCCCC ? ((uint8_t*)ptr - 8) : ((uint8_t*)ptr - 16);
	if (size)
		*size = *(size_t*)mem;
	return mem;
}

void crt::heap_initialize()
{
#ifdef _WIN64
	//RtlGetProcessHeaps(1, &state.h_heap);

	const auto peb = __readgsqword(0x60);
	state.h_heap = *(HANDLE*)(peb + 0x30);

	return;
#endif
	state.h_heap = GetProcessHeap(); //HeapCreate(0, 0, 0);//
}

static void* aligned_alloc(size_t size)
{
	// allocate memory of size bytes
	const auto mem = HeapAlloc(state.h_heap, 0, size + 16);
	return get_ptr_from_mem(mem, size);
}

void* crt::alloc(size_t size, size_t alignment)
{
#ifdef _WIN64
	return RtlAllocateHeap(state.h_heap, 0, size);
#endif

	if (alignment == 16)
	{
		return aligned_alloc(size);
	}

	return HeapAlloc(state.h_heap, 0, size);
}

static void* aligned_realloc(void* ptr, size_t size)
{
	if (ptr)
	{
		size_t user_data_size;
		get_mem_from_ptr(ptr, &user_data_size);

		const auto new_mem = crt::alloc(size, 16);
		memcpy(new_mem, ptr, user_data_size);
		crt::free(ptr, 16);

		return new_mem;
	}

	return nullptr;
}

// copy old memory from ptr to newly allocated memory of size "size", effectively increasing the capacity 
void* crt::realloc(void* ptr, size_t size, size_t alignment)
{
#ifdef _WIN64
	return RtlReAllocateHeap(state.h_heap, 0, ptr, size);
#endif
	if (alignment == 16)
	{
		return aligned_realloc(ptr, size);
	}

	return HeapReAlloc(state.h_heap, 0, ptr, size);
}

static void aligned_free(void* ptr)
{
	const auto mem = get_mem_from_ptr(ptr);
	HeapFree(state.h_heap, 0, mem);
}

void crt::free(void* ptr, size_t alignment)
{
#ifdef _WIN64
	RtlFreeHeap(state.h_heap, 0, ptr);
	return;
#endif
	if (ptr)
	{
		if (alignment == 16)
		{
			return aligned_free(ptr);
		}

		HeapFree(state.h_heap, 0, ptr);
	}
}


void* operator new(size_t s)
{
	return crt::alloc(s);
}

void* operator new[](size_t s)
{
	return crt::alloc(s);
}

void* operator new  (size_t count, std::align_val_t al)
{
	return crt::alloc(count, static_cast<size_t>(al));
}

void* operator new[](size_t count, std::align_val_t al)
{
	return crt::alloc(count, static_cast<size_t>(al));
}

void* operator new (size_t count, void* ptr) noexcept
{
	return ptr;
}

void* operator new[](size_t count, void* ptr) noexcept
{
	return ptr;
}

void operator delete(void* p) noexcept
{
	return crt::free(p);
}

void operator delete(void* p, size_t s) noexcept
{
	return crt::free(p);
}

void operator delete[](void* p) noexcept
{
	return crt::free(p);
}

void operator delete[](void* p, std::align_val_t al) noexcept
{
	return crt::free(p, static_cast<size_t>(al));
}

void operator delete(void*p, std::align_val_t al) noexcept
{
	return crt::free(p, static_cast<size_t>(al));
}

void operator delete(void* p, size_t size, std::align_val_t al) noexcept
{
	return crt::free(p, static_cast<size_t>(al));
}

void operator delete[](void* p, size_t size, std::align_val_t al) noexcept
{
	return crt::free(p, static_cast<size_t>(al));
}

void operator delete[](void* p, size_t s) noexcept
{
	return crt::free(p);
}