#pragma once

#ifndef __Z8000_H__
#define __Z8000_H__


enum
{
	Z8000_PC=1, Z8000_NSP, Z8000_FCW, Z8000_PSAP, Z8000_REFRESH,
	Z8000_IRQ_REQ, Z8000_IRQ_SRV, Z8000_IRQ_VEC,
	Z8000_R0, Z8000_R1, Z8000_R2, Z8000_R3,
	Z8000_R4, Z8000_R5, Z8000_R6, Z8000_R7,
	Z8000_R8, Z8000_R9, Z8000_R10, Z8000_R11,
	Z8000_R12, Z8000_R13, Z8000_R14, Z8000_R15
};

/* Interrupt Types that can be generated by outside sources */
#define Z8000_EPU		0x8000	/* extended instruction trap */
#define Z8000_TRAP		0x4000	/* privileged instruction trap */
#define Z8000_NMI		0x2000	/* non maskable interrupt */
#define Z8000_SEGTRAP	0x1000	/* segment trap (Z8001) */
#define Z8000_NVI		0x0800	/* non vectored interrupt */
#define Z8000_VI		0x0400	/* vectored interrupt (LSB is vector)  */
#define Z8000_SYSCALL	0x0200	/* system call (lsb is vector) */
#define Z8000_HALT		0x0100	/* halted flag  */

/* PUBLIC FUNCTIONS */
DECLARE_LEGACY_CPU_DEVICE(Z8001, z8001);
DECLARE_LEGACY_CPU_DEVICE(Z8002, z8002);

CPU_DISASSEMBLE( z8000 );

#endif /* __Z8000_H__ */
