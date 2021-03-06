// Heap.cpp - Kernel heap management

#include <Heap.h>
#include <cstddef>
#include <new>

void* operator new(size_t count)
{
	return Kernel::Heap::Alloc(count);
}

void* operator new[](size_t count)
{
	return Kernel::Heap::Alloc(count);
}

void operator delete(void* pointer) noexcept
{
	Kernel::Heap::Free(pointer);
}

void operator delete[](void* pointer) noexcept
{
	Kernel::Heap::Free(pointer);
}

void operator delete(void* pointer, size_t size __attribute__((unused)))
{
	Kernel::Heap::Free(pointer);
}

void operator delete[](void* pointer, size_t size __attribute__((unused)))
{
	Kernel::Heap::Free(pointer);
}
