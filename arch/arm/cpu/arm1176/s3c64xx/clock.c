/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/clk.h>

#define CLK_A	0
#define CLK_M	1
#define CLK_P	2

#ifndef CONFIG_SYS_CLK_FREQ_6410
#define CONFIG_SYS_CLK_FREQ_6410	12000000
#endif

unsigned long get_pll_clk(int pllreg)
{
	struct s3c6410_clock *clk =
		(struct s3c6410_clock *)samsung_get_base_clock();
	unsigned long r, m, p, s, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con);
		break;
	case MPLL:
		r = readl(&clk->mpll_con);
		break;
	case EPLL:
		r = readl(&clk->epll_con0);
		break;
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 */

	m = (r >> 16) & 0x3ff;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	/* FOUT = MDIV * FIN / (PDIV * 2^SDIV) */
	freq = CONFIG_SYS_CLK_FREQ_6410;
	fout = m * (freq / (p * (1 << s)));

	return fout;
}

unsigned long get_arm_clk(void)
{
	struct s3c6410_clock *clk =
		(struct s3c6410_clock *)samsung_get_base_clock();
	unsigned long div;
	unsigned long armclk;

	div = readl(&clk->div0);

	armclk = get_pll_clk(APLL) / ((div&0x07) + 1);

	return armclk;
}

/* s3c6410: return HCLK frequency */
unsigned long get_hclk(void)
{
	struct s3c6410_clock *clk =
		(struct s3c6410_clock *)samsung_get_base_clock();
	uint reg_others;
	uint div0, div1;
	ulong fclk;

	reg_others =  (*(vu_long *)(0x7000f900));
	if (reg_others&0x80)
		fclk = get_pll_clk(APLL);
	else
		fclk = get_pll_clk(MPLL);
	
	div0 = readl(&clk->div0);
	/* D0_BUS_RATIO: [10:8] */
	div1 = ((div0 >> 8) & 0x1)+1;
	div0 =( (div0>>9)&0x7)+1;

	fclk = fclk/ (div0*div1);

	return fclk;
}

/* s3c6410: return peripheral clock frequency */
unsigned long get_pclk(void)
{
	struct s3c6410_clock *clk =
		(struct s3c6410_clock *)samsung_get_base_clock();
	uint reg_others;
	uint div0, div1;
	ulong fclk;

	reg_others =  (*(vu_long *)(0x7000f900));
	if (reg_others&0x80)
		fclk = get_pll_clk(APLL);
	else
		fclk = get_pll_clk(MPLL);
	
	div0 = readl(&clk->div0);
	/* D0_BUS_RATIO: [10:8] */
	div1 =( (div0 >> 12) & 0xf)+1;
	div0 = ((div0>>9)&0x7)+1;

	fclk = fclk / (div0*div1);

	return fclk;
}

ulong get_uclk(void)
{
	return (get_pll_clk(EPLL));
}

