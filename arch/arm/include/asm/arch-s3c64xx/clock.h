/*
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ASM_ARM_ARCH_CLOCK_H_
#define __ASM_ARM_ARCH_CLOCK_H_

#ifndef __ASSEMBLY__
struct s3c6410_clock {
	unsigned int	apll_lock;
	unsigned int	mpll_lock;
	unsigned int	epll_lock;
	unsigned int	apll_con;
	unsigned int	mpll_con;
	unsigned int	epll_con0;
	unsigned int	epll_con1;
	unsigned int	src1;
	unsigned int	div0;
	unsigned int	div1;
	unsigned int	div2;
	unsigned int	out;
	unsigned int	hclk_gate;
	unsigned int	pclk_gate;
	unsigned int	sclk_gate;
	unsigned int	mem0_gate;
	unsigned char res[48];
	unsigned int	ahb_con0;
	unsigned int	ahb_con1;
	unsigned int	ahb_con2;
	unsigned int	src2;
	unsigned char	res1[4];;
	unsigned int	sys_id;
	unsigned int	sys_others;
};
#endif
#endif
