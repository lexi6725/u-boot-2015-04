/*
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _S3C64XX_CPU_H
#define _S3C64XX_CPU_H

#define S3C_CPU_NAME		"S3C"
#define S3C64XX_ADDR_BASE	0x50000000

/* S3C6410 */
#define S3C6410_PRO_ID		0x7E00F118
#define S3C6410_CLOCK_BASE	0x7E00F000
#define S3C6410_GPIO_BASE	0x7F008000
#define S3C6410_VIC0_BASE	0x71200000
#define S3C6410_VIC1_BASE	0x71300000
//#define S3C6410_VIC2_BASE	0xE4200000
#define S3C6410_DMC_BASE	0x7E001000
#define S3C6410_SROMC_BASE	0x70000000
#define S3C6410_ONENAND_BASE	0x70100000
#define S3C6410_PWMTIMER_BASE	0x7F006000
#define S3C6410_WATCHDOG_BASE	0x7E004000
#define S3C6410_UART_BASE	0x7F005000
#define S3C6410_MMC_BASE	0x7C200000

#ifndef __ASSEMBLY__
#include <asm/io.h>
/* CPU detection macros */
extern unsigned int s3c_cpu_id;
extern unsigned int s3c_cpu_rev;

static inline int s3c_get_cpu_rev(void)
{
	return s3c_cpu_rev;
}

static inline void s3c_set_cpu_id(void)
{
	s3c_cpu_id = readl(S3C6410_PRO_ID);
	s3c_cpu_rev = s3c_cpu_id & 0x000000FF;
	s3c_cpu_id = 0xC000 | ((s3c_cpu_id & 0x0FFFF000) >> 12);
}

static inline char *s3c_get_cpu_name(void)
{
	return S3C_CPU_NAME;
}

#define SAMSUNG_BASE(device, base)				\
static inline unsigned int samsung_get_base_##device(void)	\
{								\
	return S3C6410_##base;				\
}

SAMSUNG_BASE(clock, CLOCK_BASE)
SAMSUNG_BASE(gpio, GPIO_BASE)
SAMSUNG_BASE(pro_id, PRO_ID)
SAMSUNG_BASE(mmc, MMC_BASE)
SAMSUNG_BASE(sromc, SROMC_BASE)
SAMSUNG_BASE(timer, PWMTIMER_BASE)
SAMSUNG_BASE(uart, UART_BASE)
SAMSUNG_BASE(watchdog, WATCHDOG_BASE)
#endif

#endif	/* _S3C64XX_CPU_H */
