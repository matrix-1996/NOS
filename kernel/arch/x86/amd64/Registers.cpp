// Registers.cpp - Register dump pushed on interrupt / exception.

#include INC_SUBARCH(Registers.h)
#include INC_ARCH(ControlRegisters.h)
#include <Console.h>

using namespace Kernel;

void RegisterSet::Dump(void)
{
	Console::WriteFormat(" CS = 0x%16lx; RIP = 0x%16lx\n", cs, rip);
	Console::WriteFormat("RAX = 0x%16lx; RBX = 0x%16lx\n", rax, rbx);
	Console::WriteFormat("RCX = 0x%16lx; RDX = 0x%16lx\n", rcx, rdx);
	Console::WriteFormat("RSI = 0x%16lx; RDI = 0x%16lx\n", rsi, rdi);
	Console::WriteFormat("RBP = 0x%16lx; RSP = 0x%16lx\n", rbp, rsp);
	Console::WriteFormat("R8  = 0x%16lx; R9  = 0x%16lx\n", r8, r9);
	Console::WriteFormat("R10 = 0x%16lx; R11 = 0x%16lx\n", r10, r11);
	Console::WriteFormat("R12 = 0x%16lx; R13 = 0x%16lx\n", r12, r13);
	Console::WriteFormat("R14 = 0x%16lx; R15 = 0x%16lx\n", r14, r15);
	Console::WriteFormat("CR3 = 0x%16lx; FLG = 0x%16lx\n", CR3::Read(), rflags);
}

void SelectorErrorCode::Dump(void)
{
	Console::WriteFormat("%s, %s, selector 0x%4x\n", (External ? "external" : "internal"), (DescriptorIDT ? "IDT" : (DescriptorLDT ? "LDT" : "GDT")), SelectorIndex << 3);
}

void PageFaultErrorCode::Dump(void)
{
	Console::WriteFormat("CR2 = 0x%16lx; %s, %s,\n", CR2::Read(), (Present ? "protection violation" : "non-present page"), (WriteAccess ? "write" : "read"));
	Console::WriteFormat("%s, %s, %s\n", (UserMode ? "user" : "supervisor"), (ReservedBits ? "reserved bits" : "no reserved bits"), (InstructionFetch ? "instruction fetch" : "data access"));
}
