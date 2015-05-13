// Pager.h - Working with page tables

#ifndef __ARCH_X86_IA32_PAGER_H__
#define __ARCH_X86_IA32_PAGER_H__

#include <StdDef.h>
#include INC_ARCH(Multiboot.h)
#include <PhysicalMemory.h>

namespace Kernel
{
	struct PageTable
	{
		unsigned long page[1024];
	} PACKED;

	class Pager : PhysicalMemory
	{
	private:
		PageTable* pgdirphys;
		PageTable* pgtabphys;
		PageTable* pgdirvirt;
		PageTable* pgtabvirt;

		unsigned long* bitmap;
		unsigned long bmlength;

		unsigned long totalmem;
		unsigned long memtop;
		unsigned long numpages;
		unsigned long allocend;
		unsigned long allocendphys;

		void MarkAlloc(void* first, void* last);
		void MarkFree(void* first, void* last);
		void MarkAlloc(unsigned long page);
		void MarkFree(unsigned long page);
		bool IsAlloc(unsigned long page);
		bool TableExists(unsigned long tab);
		bool TableEmpty(unsigned long tab);

		void CreateTable(unsigned long num);
		void DestroyTable(unsigned long num);

	public:
		static const unsigned long PAGE_PRESENT       = 0x00000001;
		static const unsigned long PAGE_READ_WRITE    = 0x00000002;
		static const unsigned long PAGE_P3_ACCESS     = 0x00000004;
		static const unsigned long PAGE_WRITE_THROUGH = 0x00000008;
		static const unsigned long PAGE_CACHE_DISABLE = 0x00000010;
		static const unsigned long PAGE_ACCESSED      = 0x00000020;
		static const unsigned long PAGE_DIRTY         = 0x00000040;
		static const unsigned long PAGE_SIZE_4MB      = 0x00000080;
		static const unsigned long PAGE_GLOBAL        = 0x00000100;

		static const unsigned long PAGE_SIZE       = 4096;
		static const unsigned long PAGE_SIZE_MASK  = 4095;
		static const unsigned long PAGE_SIZE_SHIFT =   12;

		static const unsigned char AREA_ARBITRARY  = 0;
		static const unsigned char AREA_BELOW_16MB = 1;
		static const unsigned char AREA_BELOW_1MB  = 2;

		Pager(MultibootInfo* mbi);

		void* AllocBlocks(void* virt, unsigned long n, unsigned char area, bool cont);
		void* AllocBlocks(void* virt, unsigned long n);
		void FreeBlocks(void* virt, unsigned long n);

		unsigned long GetBlockSize(void);

		bool VirtExists(void* addr);
		unsigned long VirtToPhys(void* addr);
		void* PhysToVirt(unsigned long addr);
		void* MapToDefault(void* first, void* last);
		void* MapToLinear(void* phys, void* virt, unsigned long n);
		void Unmap(void* virt, unsigned long n);

		PageTable* CreateContext(void);
		unsigned long SwitchContext(unsigned long phys);
		void CopyToContext(unsigned long phys, void* src, void* dst, unsigned long length);
		void FillContext(unsigned long phys, unsigned char fill, void* dst, unsigned long length);
		void DeleteContext(PageTable* pg);
	};
}

inline Kernel::Pager& pager(void) { return(reinterpret_cast<Kernel::Pager&>(physmem_space)); }

extern "C" Kernel::PageTable tabPGDIR;
extern "C" Kernel::PageTable tabPGTAB;
extern "C" unsigned long tabBITS[];

#endif