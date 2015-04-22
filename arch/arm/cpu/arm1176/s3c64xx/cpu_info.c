/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/clk.h>

/* Default is s3c6410 */
unsigned int s3c_cpu_id = 0x6410;
/* Default is EVT1 */
unsigned int s3c_cpu_rev = 5;

#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	s3c_set_cpu_id();

	return 0;
}
#endif

u32 get_device_type(void)
{
	return s3c_cpu_id;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	char buf[32];

	printf("CPU:\t%s%X@%sMHz\n",
			s3c_get_cpu_name(), s3c_cpu_id,
			strmhz(buf, get_arm_clk()));

	return 0;
}
#endif
