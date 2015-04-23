/*
 * Copyright (C) 2009 Samsung Electronics
 * Kyungmin Park <kyungmin.park@samsung.com>
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <asm/arch/cpu.h>
#include <asm/arch/power.h>

/*
 * Register usages:
 *
 * r5 has zero always
 */

	.globl lowlevel_init
lowlevel_init:
	mov	r9, lr

	ldr	r0, =0x7f008000
	ldr	r1, =0x55555555
	str	r1, [r0, #0x800]

	ldr	r1, =0x55555555
	str	r1, [r0, #0x804]

	ldr	r1, = 0x22222666
	str	r1, [r0, #0x810]

	ldr	r1, =0x4000000
	str	r1, [r0, #0xA0]
	ldr	r1, =0x2000
	str	r1, [r0, #0xA4]

	ldr	r0, =0x7f008000
	ldr	r1, = 0x00111111
	str	r1, [r0, #0x820]

	ldr	r1, =0x00000555
	str	r1, [r0, #0x828]

	ldr	r1, =0x002A
	str	r1, [r0, #0x824]

	ldr	r1, =0
	str	r1, [r0, #0x1D4]
	
	/* Disable Watchdog */
	ldr	r0, =0x7e000000		@0x7e004000
	orr	r0, r0, #0x4000
	mov	r1, #0
	str	r1, [r0]

	@ External interrupt pending clear
	ldr	r0, =0x7F008924	/*EINTPEND*/
	ldr	r1, [r0]
	str	r1, [r0]

	ldr	r0, =0x71200000 	@0x71200000
	ldr	r1, =0x71300000 	@0x71300000
	
	@ Disable all interrupts (VIC0 and VIC1)
	mvn r3, #0x0
	str r3, [r0, #0x14]
	str r3, [r1, #0x14]
	
	@ Set all interrupts as IRQ
	mov r3, #0x0
	str r3, [r0, #0x0C]
	str r3, [r1, #0x0C]
	
	@ Pending Interrupt Clear
	mov r3, #0x0
	str r3, [r0, #0xF00]
	str r3, [r1, #0xF00]

	/* init system clock */
	bl system_clock_init
	
	/* for UART */
	bl uart_asm_init

	/* for Nand */
	bl nand_asm_init
1:
	mov	lr, r9
	mov	pc, lr

/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
system_clock_init:
	ldr r0, =0x7e00f000 @0x7e00f000

	ldr r1, [r0, #0x900]
	mov r2, #0x40
	orr r1, r1, r2
	str r1, [r0, #0x900]

	nop
	nop
	nop
	nop
	nop

	ldr r2, =0x80
	orr r1, r1, r2
	str r1, [r0, #0x900]

check_syncack:
	ldr r1, [r0, #0x900]
	ldr r2, =0xf00
	and r1, r1, r2
	cmp r1, #0xf00
	bne check_syncack

	mov r1, #0xff00
	orr r1, r1, #0xff
	str r1, [r0, #0x00]
	str r1, [r0, #0x04]
	str r1, [r0, #0x08]
/* CLKUART(=66.5Mhz) = CLKUART_input(532/2=266Mhz) / (UART_RATIO(3)+1) */
/* CLKUART(=50Mhz) = CLKUART_input(400/2=200Mhz) / (UART_RATIO(3)+1) */
/* Now, When you use UART CLK SRC by EXT_UCLK1, We support 532MHz & 400MHz value */

	ldr 	r1, [r0, #0x28]
	bic r1, r1, #0x70000
	orr r1, r1, #0x30000
	str r1, [r0, #0x28]

	ldr 	r1, [r0, #0x20]	/*Set Clock Divider*/
	bic r1, r1, #0x30000
	bic r1, r1, #0xff00
	bic r1, r1, #0xff
	ldr r2, =CLK_DIV_VAL
	orr r1, r1, r2
	str r1, [r0, #0x20]

	ldr r1, =APLL_VAL
	str r1, [r0, #0x0C]
	ldr r1, =MPLL_VAL
	str r1, [r0, #0x10]

	ldr r1, =0x80200103 		/* FOUT of EPLL is 96MHz */
	str r1, [r0, #0x14]
	ldr r1, =0x0
	str r1, [r0, #0x18]

	ldr r1, [r0, #0x1C]	/* APLL, MPLL, EPLL select to Fout */

	ldr r2, =0x2007
	orr r1, r1, r2

	str r1, [r0, #0x1C]

	/* wait at least 200us to stablize all clock */
	mov r1, #0x10000
1:	subs	r1, r1, #1
	bne 1b

	ldr r1, [r0, #0x900]
	orr r1, r1, #0x20
	str r1, [r0, #0x900]
	mov pc, lr


/*
 * uart_asm_init: Initialize UART's pins
 */
uart_asm_init:
	/* set GPIO to enable UART */
	@ GPIO setting for UART
	ldr r0, =0x7F008000
	ldr r1, =0x220022
	str 	r1, [r0, #0x00]

	ldr r1, =0x2222
	str 	r1, [r0, #0x20]

	ldr r0, =0x7F005000		@0x7F005000
	mov r1, #0x0
	str r1, [r0, #0x08]
	str r1, [r0, #0x0C]

	mov r1, #0x3					@was 0.
	str r1, [r0, #0x00]

	ldr r1, =0xe45			/* UARTCLK SRC = 11 => EXT_UCLK1*/

	str r1, [r0, #0x04]

	ldr r1, =0x22
	str r1, [r0, #0x28]

	ldr r1, =0x1FFF
	str r1, [r0, #0x2C]

	ldr r1, =0x4f4f4f4f
	str r1, [r0, #0x20]		@'O'

	mov pc, lr


/*
 * nand_asm_init: Initialize Nand Flash
 */
nand_asm_init:
	ldr r0, =0x70200000
	ldr r1, [r0, #0x00]
	orr r1, r1, #0x70
	orr r1, r1, #0x7700
	str 	r1, [r0, #0x00]
	
	ldr r1, [r0, #0x04]
	orr r1, r1, #0x03
	str 	r1, [r0, #0x04]
	
	mov pc, lr

