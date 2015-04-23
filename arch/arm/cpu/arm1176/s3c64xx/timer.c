/*
 * Copyright (C) 2009 Samsung Electronics
 * Heungjun Kim <riverful.kim@samsung.com>
 * Inki Dae <inki.dae@samsung.com>
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <div64.h>
#include <asm/io.h>
#include <asm/arch/pwm.h>
#include <asm/arch/clk.h>
#include <pwm.h>

DECLARE_GLOBAL_DATA_PTR;

unsigned long get_current_tick(void);

/* macro to read the 16 bit timer */
static inline struct s3c_timer *s3c_get_base_timer(void)
{
	return (struct s3c_timer *)samsung_get_base_timer();
}

/**
 * Read the countdown timer.
 *
 * This operates at 1MHz and counts downwards. It will wrap about every
 * hour (2^32 microseconds).
 *
 * @return current value of timer
 */
static unsigned long timer_get_us_down(void)
{
	struct s3c_timer *const timer = s3c_get_base_timer();

	return readl(&timer->tcnto4);
}

int timer_init(void)
{
	struct s3c_timer *const timer = s3c_get_base_timer();
	/* PWM Timer 4 */
	timer->tcfg0	= 0x0f00;
	if (gd->arch.timer_rate_hz == 0)
	{
		gd->arch.timer_rate_hz = get_pclk()/(2*16*100);
	}
	/* Use this as the current monotonic time in us */
	gd->arch.timer_reset_value = 0;

	/* Use this as the last timer value we saw */
	gd->arch.lastinc = timer->tcntb4 = gd->arch.timer_rate_hz ;

	timer->tcon = (timer->tcon&~0x00700000) |(0x11<<21);

	return 0;
}

/*
 * timer without interrupts
 */
unsigned long get_timer(unsigned long base)
{
	unsigned long long time_ms;

	ulong now = timer_get_us_down();

	/*
	 * Increment the time by the amount elapsed since the last read.
	 * The timer may have wrapped around, but it makes no difference to
	 * our arithmetic here.
	 */
	gd->arch.timer_reset_value += gd->arch.lastinc - now;
	gd->arch.lastinc = now;

	/* Divide by 1000 to convert from us to ms */
	time_ms = gd->arch.timer_reset_value;
	do_div(time_ms, 1000);
	return time_ms - base;
}

unsigned long __attribute__((no_instrument_function)) timer_get_us(void)
{
	static unsigned long base_time_us;

	struct s3c_timer *const timer =
		(struct s3c_timer *)samsung_get_base_timer();
	unsigned long now_downward_us = readl(&timer->tcnto4);

	if (!base_time_us)
		base_time_us = now_downward_us;

	/* Note that this timer counts downward. */
	return base_time_us - now_downward_us;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	unsigned long count_value;

	count_value = timer_get_us_down();
	while ((int)(count_value - timer_get_us_down()) < (int)usec)
		;
}

void reset_timer_masked(void)
{
	struct s3c_timer *const timer = s3c_get_base_timer();

	/* reset time */
	gd->arch.lastinc = readl(&timer->tcnto4);
	gd->arch.tbl = 0;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return (gd->arch.timer_rate_hz*100);
}
