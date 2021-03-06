// Multiboot.cpp - Multiboot support for the kernel.

#include <Elf.h>
#include <Console.h>
#include <Symbol.h>
#include INC_ARCH(X86Pager.h)
#include INC_ARCH(Multiboot.h)
#include <TaskManager.h>
#include <PhysicalMemory.h>
#include <Heap.h>
#include <Chunker.h>
#include <Pager.h>
#include <new>

namespace Kernel
{
	namespace Multiboot
	{
		SECTION(".init.text") bool CheckMagic(uint32_t magic)
		{
			Console::WriteMessage((magic == MAGIC ? Console::Style::OK : Console::Style::WARNING), "Multiboot magic:", "0x%8x", magic);
			return (magic == MAGIC);
		}

		SECTION(".init.text") Info* Info::InitMemory(void)
		{
			Info* mbi = reinterpret_cast<Info*>(Pager::MapBootRegion(reinterpret_cast<Memory::PhysAddr>(this), sizeof(Info), Memory::MemType::KERNEL_RO));
//			new (physmem_space) X86Pager(this);
			MemoryMap* mmap;
			MemoryMap* mmap0;

			uint32_t mem = mbi->UpperMemory;
			uint32_t length = (mem > (15UL << 10) ? 15UL << 20 : mem << 10);
			Chunker::Init(1UL << 20, length, Memory::Zone::DMA24);
			Console::WriteFormat("Upper memory: %dkB\n", mem);

			if(mbi->Flags & FLAGS_MEMORY_MAP)
			{
				Console::WriteFormat("Memory map of length 0x%8x at address 0x%8x\n", mbi->MemoryMapLength, mbi->MemoryMapAddress);
				mmap0 = reinterpret_cast<MemoryMap*>(Pager::MapBootRegion(mbi->MemoryMapAddress, mbi->MemoryMapLength, Memory::MemType::KERNEL_RO));
				for(mmap = mmap0; (uintptr_t)mmap - (uintptr_t)mmap0 < mbi->MemoryMapLength; mmap = (MemoryMap*)((uintptr_t)mmap + mmap->Size + 4))
				{
					Console::WriteFormat("Mem: 0x%16lx-0x%16lx, Type: 0x%2x\n", mmap->BaseAddr, mmap->BaseAddr + mmap->Length - 1, mmap->Type);
					//x86pager().AddMemoryArea(mmap);
				}
			}

			Heap::Init();
			Heap::ShowMap();
			auto test = new int[256];
			Heap::ShowMap();
			auto test2 = new char[32];
			Heap::ShowMap();
			delete[] test;
			Heap::ShowMap();
			delete[] test2;
			Heap::ShowMap();

			return mbi;
		}

		SECTION(".init.text") void Info::InitModules(void)
		{
			if((Flags & FLAGS_MODULES) && (ModuleCount > 0))
			{
				Console::WriteMessage(Console::Style::INFO, "Boot modules:", "Found %d modules @ 0x%8x.", ModuleCount, ModuleAddress);
				Module* mbm = (Module*)x86pager().MapTempRegion(ModuleAddress, ModuleCount * sizeof(Module));
				for(unsigned int i = 0; i < ModuleCount; i++)
				{
					Console::WriteMessage(Console::Style::INFO, "Boot module:", "0x%8x - 0x%8x", mbm[i].ModStart, mbm[i].ModEnd);
					mbm[i].Start();
				}
			}
			else
				Console::WriteMessage(Console::Style::WARNING, "Boot modules:", "not found.");
		}

		SECTION(".init.text") bool Module::Start(void)
		{
			Elf* elf = (Elf*)x86pager().MapTempRegion(ModStart, ModEnd - ModStart + 1);
			Elf::Header* ehdr = elf->GetHeader();

			if(ehdr->Magic[0] != Elf::ELF_MAG0 || ehdr->Magic[1] != Elf::ELF_MAG1 || ehdr->Magic[2] != Elf::ELF_MAG2 || ehdr->Magic[3] != Elf::ELF_MAG3)
			{
				Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Invalid ELF.", ModStart);
				return false;
			}
#if defined ELF32
			if(ehdr->Class != Elf::ELF_CLASS_32)
			{
				Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No 32bit module.", ModStart);
				return false;
			}
#elif defined ELF64
			if(ehdr->Class != Elf::ELF_CLASS_64)
			{
				Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No 64bit module.", ModStart);
				return false;
			}
#endif
			if(ehdr->Machine != kernelElfHeader.Machine)
			{
				Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Incorrect architecture 0x%4x, expected 0x%4x.", ModStart, ehdr->Machine, kernelElfHeader.Machine);
				return false;
			}

			if(!ehdr->PHNum)
			{
				Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No program header found.", ModStart);
				return false;
			}

			if(!ehdr->SHNum)
			{
				Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No section header found.", ModStart);
				return false;
			}

			if(ehdr->Type == Elf::ET_EXEC)
			{
				Process* p = taskman().CreateProcess(elf);
				Console::WriteMessage(Console::Style::OK, "Executable Module at 0x%8x:", "Started with %d bytes.", ModStart, p->memory.Value());
				return true;
			}

			if(ehdr->Type == Elf::ET_DYN)
			{
				Console::WriteMessage(Console::Style::WARNING, "Library Module at 0x%8x:", "Not implemented.", ModStart);
				return false;
			}

			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Unknown type 0x%4x, unable to execute.", ModStart, ehdr->Type);
			return false;
		}
	}
}
/*
SECTION(".init.text") Multiboot::Multiboot(unsigned long magic, Info* info)
{
	MemoryMap* mmap;
	MemoryMap* mmap0;
	Drive* drive;
	Drive* drive0;
	unsigned int i;

	mbi = (Info*)x86pager().MapTempRegion((unsigned long)info, sizeof(Info));

	// Print some multiboot info.
	Console::WriteMessage((magic == MULTIBOOT_MAGIC ? Console::Style::OK : Console::Style::WARNING), "Multiboot Info:", "Magic: 0x%8x, Address: 0x%8x, Flags: 0x%8x", magic, info, mbi->Flags);

	if(mbi->Flags & MB_FLAGS_MEMORY_MAP)
	{
		Console::WriteFormat("Memory map of length 0x%8x at address 0x%8x\n", mbi->MemoryMapLength, mbi->MemoryMapAddress);
		mmap0 = (MemoryMap*)x86pager().MapTempRegion(mbi->MemoryMapAddress, mbi->MemoryMapLength);
		for(mmap = mmap0; (unsigned long)mmap - (unsigned long)mmap0 < mbi->MemoryMapLength; mmap = (MemoryMap*)((unsigned long)mmap + mmap->Size + 4))
		{
			Console::WriteFormat("Mem: 0x%16lx-0x%16lx, Type: 0x%2x\n", mmap->BaseAddr, mmap->BaseAddr + mmap->Length - 1, mmap->Type);
			x86pager().AddMemoryArea(mmap);
		}
	}

	if(mbi->Flags & MB_FLAGS_MEMORY_INFO)
	{
		Console::WriteMessage(Console::Style::INFO, "Base contiguous memory:", "%u kB", mbi->UpperMemory + 1024);
	}

	if(mbi->Flags & MB_FLAGS_DRIVES)
	{
		Console::WriteFormat("Drive list of length 0x%8x at address 0x%8x\n", mbi->DrivesLength, mbi->DrivesAddress);
		drive0 = (Drive*)x86pager().MapTempRegion(mbi->DrivesAddress, mbi->DrivesLength);
		for(drive = drive0; (unsigned long)drive - (unsigned long)drive0 < mbi->DrivesLength; drive = (Drive*)((unsigned long)drive + drive->Size))
			Console::WriteFormat("Number: 0x%2x, Mode: %s, C: %d, H: %d, S: %d\n", drive->Number, (drive->Mode ? "LBA" : "CHS"), drive->Cylinders, drive->Heads, drive->Sectors);
	}

	if(mbi->Flags & MB_FLAGS_BOOT_DEVICE)
		Console::WriteMessage(Console::Style::INFO, "Boot device:", "Drive: 0x%2x, Partition: %d / %d / %d", mbi->BootDrive, mbi->BootPartition1, mbi->BootPartition2, mbi->BootPartition3);

	if(mbi->Flags & MB_FLAGS_NAME)
		Console::WriteMessage(Console::Style::INFO, "Bootloader @ 0x%8x:", "%s", mbi->BootLoaderName, x86pager().MapTempString(mbi->BootLoaderName));

	if(mbi->Flags & MB_FLAGS_CMDLINE)
		Console::WriteMessage(Console::Style::INFO, "Command line @ 0x%8x:", "%s", mbi->CommandLine, x86pager().MapTempString(mbi->CommandLine));

	if(mbi->Flags & MB_FLAGS_MODULES)
	{
		Console::WriteMessage(Console::Style::INFO, "Boot modules:", "@ 0x%8x", mbi->ModuleAddress);
		mbm = (Module*)x86pager().MapTempRegion(mbi->ModuleAddress, mbi->ModuleCount * sizeof(Module));
		for(i = 0; i < mbi->ModuleCount; i++)
			Console::WriteMessage(Console::Style::INFO, "Boot module:", "0x%8x - 0x%8x", mbm[i].ModStart, mbm[i].ModEnd);
	}
}

void SECTION(".init.text") Multiboot::StartModules(void)
{
	unsigned int i;
	Elf::Header* module;
	Process* p;

	if(!(mbi->Flags & MB_FLAGS_MODULES))
		return;

	for(i = 0; i < mbi->ModuleCount; i++)
	{
		module = (Elf::Header*)x86pager().MapTempRegion(mbm[i].ModStart, mbm[i].ModEnd - mbm[i].ModStart + 1);

		if(module->Magic[0] != Elf::ELF_MAG0 || module->Magic[1] != Elf::ELF_MAG1 || module->Magic[2] != Elf::ELF_MAG2 || module->Magic[3] != Elf::ELF_MAG3)
		{
			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Invalid ELF.", mbm[i].ModStart);
			continue;
		}
#if defined ELF32
		if(module->Class != Elf::ELF_CLASS_32)
		{
			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No 32bit module.", mbm[i].ModStart);
			continue;
		}
#elif defined ELF64
		if(module->Class != Elf::ELF_CLASS_64)
		{
			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No 64bit module.", mbm[i].ModStart);
			continue;
		}
#endif
		if(module->Machine != kernelElfHeader.Machine)
		{
			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Incorrect architecture 0x%4x, expected 0x%4x.", mbm[i].ModStart, module->Machine, kernelElfHeader.Machine);
			continue;
		}

		if(!module->PHNum)
		{
			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No program header found.", mbm[i].ModStart);
			continue;
		}

		if(!module->SHNum)
		{
			Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "No section header found.", mbm[i].ModStart);
			continue;
		}

		if(module->Type == Elf::ET_EXEC)
		{
			p = taskman().CreateProcess((Elf*)module);
			Console::WriteMessage(Console::Style::OK, "Executable Module at 0x%8x:", "Started with %d bytes.", mbm[i].ModStart, p->memory.Value());
			continue;
		}

		if(module->Type == Elf::ET_DYN)
		{
			Console::WriteMessage(Console::Style::WARNING, "Library Module at 0x%8x:", "Not implemented.", mbm[i].ModStart);
			continue;
		}

		Console::WriteMessage(Console::Style::WARNING, "Module at 0x%8x:", "Unknown type 0x%4x, unable to execute.", mbm[i].ModStart, module->Type);
	}
}
*/
