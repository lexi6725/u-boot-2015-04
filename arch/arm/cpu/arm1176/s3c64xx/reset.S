/*
 * Copyright (c) 2009 Samsung Electronics.
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/cpu.h>
#include <linux/linkage.h>

#define S3C6410_SWRESET			0x7E00f114

ENTRY(reset_cpu)
	ldr	r1, =S3C6410_SWRESET
	ldr	r2, =0x6410
	str	r2, [r1]
_loop_forever:
	b	_loop_forever
ENDPROC(reset_cpu)
