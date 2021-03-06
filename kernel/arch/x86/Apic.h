// Apic.h - Advanced Programmable Interrupt Controller.

#ifndef __ARCH_X86_APIC_H__
#define __ARCH_X86_APIC_H__

#include <cstdint>

namespace Kernel
{
	/**
	 * Local Advanced Programmable Interrupt Controller.
	 * The local APIC is used for inter-processor communication and interrupt management in SMP machines. It is
	 * accessed through memory I/O. See the IA-32 Intel Architecture Software Developer's Manual, Volume 3: System
	 * Programming Guide, order number 253668, chapter 8, for detailed information on using the local APIC.
	 */
	namespace Apic
	{
		// Set to the APIC base address to the value given by the linker.
		extern "C" volatile uint32_t apic_base[];

		// Apic registers
		static const int REG_ID      = 0x0020/4; /**< Local APIC ID */
		static const int REG_VERSION = 0x0030/4; /**< Local APIC version */
		static const int REG_TPR     = 0x0080/4; /**< Task priority register */
		static const int REG_APR     = 0x0090/4; /**< Arbitration priority register */
		static const int REG_PPR     = 0x00a0/4; /**< Processor priority register */
		static const int REG_EOI     = 0x00b0/4; /**< End-of-interrupt register */
		static const int REG_LDR     = 0x00d0/4; /**< Logical destination register */
		static const int REG_DFR     = 0x00e0/4; /**< Destination format register */
		static const int REG_SIVR    = 0x00f0/4; /**< Spurious interrupt vector register */
		static const int REG_ISR     = 0x0100/4; /**< In-service register */
		static const int REG_TMR     = 0x0180/4; /**< Trigger mode register */
		static const int REG_IRR     = 0x0200/4; /**< Interrupt request register */
		static const int REG_ESR     = 0x0280/4; /**< Error status register */
		static const int REG_ICR     = 0x0300/4; /**< Interrupt command register, part 1 */
		static const int REG_ICR2    = 0x0310/4; /**< Interrupt command register, part 2 */
		static const int REG_LVTT    = 0x0320/4; /**< Local vector table (timer) */
		static const int REG_LVTPC   = 0x0340/4; /**< Local vector table (performance counter) */
		static const int REG_LVT0    = 0x0350/4; /**< Local vector table (LINT0) */
		static const int REG_LVT1    = 0x0360/4; /**< Local vector table (LINT1) */
		static const int REG_LVTERR  = 0x0370/4; /**< Local vector table (error) */
		static const int REG_TIC     = 0x0380/4; /**< Timer initial count register */
		static const int REG_TCC     = 0x0390/4; /**< Timer current count register */
		static const int REG_TDCR    = 0x03e0/4; /**< Timer divide configuration register */

		// Error status register
		static const uint32_t ESR_SCS       = 0x01; /**< Send checksum error */
		static const uint32_t ESR_RCS       = 0x02; /**< Receive checksum error */
		static const uint32_t ESR_SACC      = 0x04; /**< Send accept error */
		static const uint32_t ESR_RACC      = 0x08; /**< Receive accept error */
		static const uint32_t ESR_SIV       = 0x20; /**< Send illegal vector */
		static const uint32_t ESR_RIV       = 0x40; /**< Receive illegal vector */
		static const uint32_t ESR_ILLREG    = 0x80; /**< Illegal register address */

		// Interrupt command register
		static const uint32_t DEST_FIELD    = 0x00000; /**< Specify recipient in destination field */
		static const uint32_t DEST_SELF     = 0x40000; /**< Interrupting processor only */
		static const uint32_t DEST_ALLINC   = 0x80000; /**< All processors, including interrupting processor */
		static const uint32_t DEST_ALLEXC   = 0xc0000; /**< All processors, excluding interrupting processor */
		static const uint32_t INT_LEVELTRIG = 0x08000; /**< Level trigger mode */
		static const uint32_t INT_EDGETRIG  = 0x00000; /**< Edge trigger mode */
		static const uint32_t INT_ASSERT    = 0x04000; /**< Assert level */
		static const uint32_t INT_PENDING   = 0x01000; /**< Interrupt pending */
		static const uint32_t DEST_LOGICAL  = 0x00800; /**< Logical address mode */
		static const uint32_t DEST_PHYSICAL = 0x00000; /**< Physical address mode */
		static const uint32_t DM_FIXED      = 0x00000; /**< Fixed */
		static const uint32_t DM_LOWPRI     = 0x00100; /**< Lowest priority */
		static const uint32_t DM_SMI        = 0x00200; /**< System management interrupt */
		static const uint32_t DM_NMI        = 0x00400; /**< Non-maskable interrupt */
		static const uint32_t DM_INIT       = 0x00500; /**< INIT */
		static const uint32_t DM_STARTUP    = 0x00600; /**< STARTUP */
		static const uint32_t DM_EXTINT     = 0x00700; /**< External interrupt */

		// Local vector table
		static const uint32_t LVT_TPER      = 0x20000; /**< Generate periodic timer interrupt */
		static const uint32_t LVT_MASKED    = 0x10000; /**< Mask local vector */
		static const uint32_t LVT_RIRR      = 0x04000; /**< Remote interrupt request */
		static const uint32_t LVT_ACTLOW    = 0x02000; /**< Interrupt input pin polarity active low */

		// Timer divisor register
		static const uint32_t TDR_1         = 0x0b; /**< Divide by 1 */
		static const uint32_t TDR_2         = 0x00; /**< Divide by 2 */
		static const uint32_t TDR_4         = 0x01; /**< Divide by 4 */
		static const uint32_t TDR_8         = 0x02; /**< Divide by 8 */
		static const uint32_t TDR_16        = 0x03; /**< Divide by 16 */
		static const uint32_t TDR_32        = 0x08; /**< Divide by 32 */
		static const uint32_t TDR_64        = 0x09; /**< Divide by 64 */
		static const uint32_t TDR_128       = 0x0a; /**< Divide by 128 */

		/**
		 * Initialize APIC at given location.
		 */
		void Init(unsigned long phys = 0xfee00000);

		/**
		 * Get local APIC physical ID.
		 * @return physical APIC ID of the processor this code is running at.
		 */
		unsigned char GetPhysID(void);

		/**
		 * Set local APIC physical ID.
		 */
		void SetPhysID(unsigned char id);

		/**
		 * Get local APIC version.
		 * @return local APIC version of the processor this code is running at.
		 */
		unsigned char GetVersion(void);

		/**
		 * Get local APIC logical ID.
		 * @return logical APIC ID of the processor this code is running at.
		 */
		unsigned char GetLogID(void);

		/**
		 * Set local APIC logical ID.
		 */
		void SetLogID(unsigned char id);

		/**
		 * Send inter-processor interrupt.
		 */
		void SendIPI(unsigned char target, unsigned char vector, unsigned int flags);

		/**
		 * Check whether send is still in progress.
		 */
		bool SendPending(void);

		/**
		 * Signal end-of-interrupt (EOI) signal.
		 */
		void SignalEOI(void);

		// Timer functions
		void TimerStart(unsigned int s, bool interrupt = true); /**< Start periodic timer. */
		void TimerStop(void); /**< Stop timer. */
		void TimerShot(unsigned int s, bool interrupt = true); /**< Start one-shot timer. */
		bool TimerPending(void); /**< Is timer interrupt pending? */
		unsigned char GetTimerVector(void); /**< Get timer interrupt vector. */
		void SetTimerVector(unsigned char vector); /**< Set timer interrupt vector. */
		unsigned char GetTimerDiv(void); /**< Get timer divisor. */
		void SetTimerDiv(unsigned char div); /**< Set timer divisor. */
		unsigned int GetTimerStart(void); /**< Get timer reload value. */
		unsigned int GetTimerCount(void); /**< Get current timer count. */

		// Error handling
		unsigned char GetErrorVector(void); /**< Get local APIC error vector. */
		void SetErrorVector(unsigned char vector); /**< Set local APIC error vector. */
		void MaskError(void); /**< Mask error interrupt. */
		void UnmaskError(void); /**< Unmask error interrupt. */
		bool ErrorPending(void); /**< Is error interrupt pending? */
		unsigned int GetErrorStatus(void); /**< Get type of error. */
		void ClearError(void); /**< Reset error state. */

		void Enable(void);
		void Disable(void);
	}
}

#endif
