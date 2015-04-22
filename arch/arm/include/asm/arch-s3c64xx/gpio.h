/*
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ASM_ARCH_GPIO_H
#define __ASM_ARCH_GPIO_H

#ifndef __ASSEMBLY__
struct s5p_gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pull;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pull;
	unsigned char	res1[8];
};

/* A list of valid GPIO numbers for the asm-generic/gpio.h interface */
enum s5pc100_gpio_pin {
	S3C6410_GPIO_A0,
	S3C6410_GPIO_A1,
	S3C6410_GPIO_A2,
	S3C6410_GPIO_A3,
	S3C6410_GPIO_A4,
	S3C6410_GPIO_A5,
	S3C6410_GPIO_A6,
	S3C6410_GPIO_A7,
	S3C6410_GPIO_B0,
	S3C6410_GPIO_B1,
	S3C6410_GPIO_B2,
	S3C6410_GPIO_B3,
	S3C6410_GPIO_B4,
	S3C6410_GPIO_B5,
	S3C6410_GPIO_B6,
	S3C6410_GPIO_C0,
	S3C6410_GPIO_C1,
	S3C6410_GPIO_C2,
	S3C6410_GPIO_C3,
	S3C6410_GPIO_C4,
	S3C6410_GPIO_C5,
	S3C6410_GPIO_C6,
	S3C6410_GPIO_C7,
	S3C6410_GPIO_D0,
	S3C6410_GPIO_D1,
	S3C6410_GPIO_D2,
	S3C6410_GPIO_D3,
	S3C6410_GPIO_D4,
	S3C6410_GPIO_E0,
	S3C6410_GPIO_E1,
	S3C6410_GPIO_E2,
	S3C6410_GPIO_E3,
	S3C6410_GPIO_E4,
	S3C6410_GPIO_F00,
	S3C6410_GPIO_F01,
	S3C6410_GPIO_F02,
	S3C6410_GPIO_F03,
	S3C6410_GPIO_F04,
	S3C6410_GPIO_F05,
	S3C6410_GPIO_F06,
	S3C6410_GPIO_F07,
	S3C6410_GPIO_F10,
	S3C6410_GPIO_F11,
	S3C6410_GPIO_F12,
	S3C6410_GPIO_F13,
	S3C6410_GPIO_F14,
	S3C6410_GPIO_F15,
	S3C6410_GPIO_F16,
	S3C6410_GPIO_F17,
	S3C6410_GPIO_G0,
	S3C6410_GPIO_G1,
	S3C6410_GPIO_G2,
	S3C6410_GPIO_G3,
	S3C6410_GPIO_G4,
	S3C6410_GPIO_G5,
	S3C6410_GPIO_G6,
	S3C6410_GPIO_H0,
	S3C6410_GPIO_H1,
	S3C6410_GPIO_H2,
	S3C6410_GPIO_H3,
	S3C6410_GPIO_H4,
	S3C6410_GPIO_H5,
	S3C6410_GPIO_H6,
	S3C6410_GPIO_H7,
	S3C6410_GPIO_H8,
	S3C6410_GPIO_H9,
	S3C6410_GPIO_I00,
	S3C6410_GPIO_I01,
	S3C6410_GPIO_I02,
	S3C6410_GPIO_I03,
	S3C6410_GPIO_I04,
	S3C6410_GPIO_I05,
	S3C6410_GPIO_I06,
	S3C6410_GPIO_I07,
	S3C6410_GPIO_I10,
	S3C6410_GPIO_I11,
	S3C6410_GPIO_I12,
	S3C6410_GPIO_I13,
	S3C6410_GPIO_I14,
	S3C6410_GPIO_I15,
	S3C6410_GPIO_I16,
	S3C6410_GPIO_I17,
	S3C6410_GPIO_J00,
	S3C6410_GPIO_J01,
	S3C6410_GPIO_J02,
	S3C6410_GPIO_J03,
	S3C6410_GPIO_J04,
	S3C6410_GPIO_J05,
	S3C6410_GPIO_J10,
	S3C6410_GPIO_J11,
	S3C6410_GPIO_J12,
	S3C6410_GPIO_J13,
	S3C6410_GPIO_J14,
	S3C6410_GPIO_J15,
	S3C6410_GPIO_K00,
	S3C6410_GPIO_K01,
	S3C6410_GPIO_K02,
	S3C6410_GPIO_K03,
	S3C6410_GPIO_K04,
	S3C6410_GPIO_K05,
	S3C6410_GPIO_K06,
	S3C6410_GPIO_K07,
	S3C6410_GPIO_K10,
	S3C6410_GPIO_K11,
	S3C6410_GPIO_K12,
	S3C6410_GPIO_K13,
	S3C6410_GPIO_K14,
	S3C6410_GPIO_K15,
	S3C6410_GPIO_K16,
	S3C6410_GPIO_K17,
	S3C6410_GPIO_L00,
	S3C6410_GPIO_L01,
	S3C6410_GPIO_L02,
	S3C6410_GPIO_L03,
	S3C6410_GPIO_L04,
	S3C6410_GPIO_L05,
	S3C6410_GPIO_L06,
	S3C6410_GPIO_L07,
	S3C6410_GPIO_L10,
	S3C6410_GPIO_L11,
	S3C6410_GPIO_L12,
	S3C6410_GPIO_L13,
	S3C6410_GPIO_L14,
	S3C6410_GPIO_L15,
	S3C6410_GPIO_L16,
	S3C6410_GPIO_M0,
	S3C6410_GPIO_M1,
	S3C6410_GPIO_M2,
	S3C6410_GPIO_M3,
	S3C6410_GPIO_M4,
	S3C6410_GPIO_M5,
	S3C6410_GPIO_N00,
	S3C6410_GPIO_N01,
	S3C6410_GPIO_N02,
	S3C6410_GPIO_N03,
	S3C6410_GPIO_N04,
	S3C6410_GPIO_N05,
	S3C6410_GPIO_N06,
	S3C6410_GPIO_N07,
	S3C6410_GPIO_N10,
	S3C6410_GPIO_N11,
	S3C6410_GPIO_N12,
	S3C6410_GPIO_N13,
	S3C6410_GPIO_N14,
	S3C6410_GPIO_N15,
	S3C6410_GPIO_N16,
	S3C6410_GPIO_N17,
	S3C6410_GPIO_O00,
	S3C6410_GPIO_O01,
	S3C6410_GPIO_O02,
	S3C6410_GPIO_O03,
	S3C6410_GPIO_O04,
	S3C6410_GPIO_O05,
	S3C6410_GPIO_O06,
	S3C6410_GPIO_O07,
	S3C6410_GPIO_O10,
	S3C6410_GPIO_O11,
	S3C6410_GPIO_O12,
	S3C6410_GPIO_O13,
	S3C6410_GPIO_O14,
	S3C6410_GPIO_O15,
	S3C6410_GPIO_O16,
	S3C6410_GPIO_O17,
	S3C6410_GPIO_P00,
	S3C6410_GPIO_P01,
	S3C6410_GPIO_P02,
	S3C6410_GPIO_P03,
	S3C6410_GPIO_P04,
	S3C6410_GPIO_P05,
	S3C6410_GPIO_P06,
	S3C6410_GPIO_P07,
	S3C6410_GPIO_P10,
	S3C6410_GPIO_P11,
	S3C6410_GPIO_P12,
	S3C6410_GPIO_P13,
	S3C6410_GPIO_P14,
	S3C6410_GPIO_P15,
	S3C6410_GPIO_P16,
	S3C6410_GPIO_Q0,
	S3C6410_GPIO_Q1,
	S3C6410_GPIO_Q3,
	S3C6410_GPIO_Q4,
	S3C6410_GPIO_Q5,
	S3C6410_GPIO_Q6,
	S3C6410_GPIO_Q7,
	S3C6410_GPIO_Q8,

	S3C6410_GPIO_MAX_PORT
};

struct gpio_info {
	unsigned int reg_addr;	/* Address of register for this part */
	unsigned int max_gpio;	/* Maximum GPIO in this part */
};

#define S3C6410_GPIO_NUM_PARTS	1
static struct gpio_info s3c6410_gpio_data[S3C6410_GPIO_NUM_PARTS] = {
	{ S3C6410_GPIO_BASE, S3C6410_GPIO_MAX_PORT },
};

static inline struct gpio_info *get_gpio_data(void)
{
	return s3c6410_gpio_data;
}

static inline unsigned int get_bank_num(void)
{
	return S3C6410_GPIO_NUM_PARTS;
}

/*
 * This structure helps mapping symbolic GPIO names into indices from
 * exynos5_gpio_pin/exynos5420_gpio_pin enums.
 *
 * By convention, symbolic GPIO name is defined as follows:
 *
 * g[p]<bank><set><bit>, where
 *   p is optional
 *   <bank> - a single character bank name, as defined by the SOC
 *   <set> - a single digit set number
 *   <bit> - bit number within the set (in 0..7 range).
 *
 * <set><bit> essentially form an octal number of the GPIO pin within the bank
 * space. On the 5420 architecture some banks' sets do not start not from zero
 * ('d' starts from 1 and 'j' starts from 4). To compensate for that and
 * maintain flat number space withoout holes, those banks use offsets to be
 * deducted from the pin number.
 */
struct gpio_name_num_table {
	char bank;		/* bank name symbol */
	u8 bank_size;		/* total number of pins in the bank */
	char bank_offset;	/* offset of the first bank's pin */
	unsigned int base;	/* index of the first bank's pin in the enum */
};

#define GPIO_PER_BANK 8
#define GPIO_ENTRY(name, base, top, offset) { name, top - base, offset, base }
static const struct gpio_name_num_table s3c6410_gpio_table[] = {
	GPIO_ENTRY('a', S3C6410_GPIO_A0, S3C6410_GPIO_B0, 0),
	GPIO_ENTRY('b', S3C6410_GPIO_B0, S3C6410_GPIO_C0, 0),
	GPIO_ENTRY('c', S3C6410_GPIO_C0, S3C6410_GPIO_D0, 0),
	GPIO_ENTRY('d', S3C6410_GPIO_D0, S3C6410_GPIO_E0, 0),
	GPIO_ENTRY('e', S3C6410_GPIO_E0, S3C6410_GPIO_F00, 0),
	GPIO_ENTRY('f', S3C6410_GPIO_F00, S3C6410_GPIO_G0, 0),
	GPIO_ENTRY('g', S3C6410_GPIO_G0, S3C6410_GPIO_H0, 0),
	GPIO_ENTRY('h', S3C6410_GPIO_H0, S3C6410_GPIO_I00, 0),
	GPIO_ENTRY('i', S3C6410_GPIO_I00, S3C6410_GPIO_J00, 0),
	GPIO_ENTRY('j', S3C6410_GPIO_J00, S3C6410_GPIO_K00, 0),
	GPIO_ENTRY('k', S3C6410_GPIO_K00, S3C6410_GPIO_L00, 0),
	GPIO_ENTRY('l', S3C6410_GPIO_L00, S3C6410_GPIO_M0, 0),
	GPIO_ENTRY('m', S3C6410_GPIO_M0, S3C6410_GPIO_N00, 0),
	GPIO_ENTRY('n', S3C6410_GPIO_N00, S3C6410_GPIO_O00, 0),
	GPIO_ENTRY('o', S3C6410_GPIO_O00, S3C6410_GPIO_P00, 0),
	GPIO_ENTRY('p', S3C6410_GPIO_P00, S3C6410_GPIO_Q0, 0),
	GPIO_ENTRY('q', S3C6410_GPIO_Q0, S3C6410_GPIO_MAX_PORT, 0),
	{ 0 }
};

/* functions */
void gpio_cfg_pin(int gpio, int cfg);
void gpio_set_pull(int gpio, int mode);
void gpio_set_drv(int gpio, int mode);
void gpio_set_rate(int gpio, int mode);
int s5p_gpio_get_pin(unsigned gpio);

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8
#endif

/* Pin configurations */
#define S3C_GPIO_INPUT	0x0
#define S3C_GPIO_OUTPUT	0x1
#define S3C_GPIO_IRQ	0x7
#define S3C_GPIO_FUNC(x)	(x)

/* Pull mode */
#define S3C_GPIO_PULL_NONE	0x0
#define S3C_GPIO_PULL_DOWN	0x1
#define S3C_GPIO_PULL_UP	0x2

/* Drive Strength level */
#define S3C_GPIO_DRV_1X	0x0
#define S3C_GPIO_DRV_3X	0x1
#define S3C_GPIO_DRV_2X	0x2
#define S3C_GPIO_DRV_4X	0x3
#define S3C_GPIO_DRV_FAST	0x0
#define S3C_GPIO_DRV_SLOW	0x1

#endif
