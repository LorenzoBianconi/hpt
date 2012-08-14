/*
 * Copyright 2010-2011	Lorenzo Bianconi <me@lorenzobianconi.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Bianconi");

/*
 * HPT macro and data structures
 */
#define US_TO_NS(x)	(x * 1000)
static struct hrtimer hp_timer;

ktime_t hp_current_time;
ktime_t hp_delta;

/* Timer period in us */
static int hp_delta_us = 25;
module_param(hp_delta_us, int, 0600);

/* Hook */
struct timeval hpt_hook(void)
{
	return ktime_to_timeval(hp_current_time);
}
EXPORT_SYMBOL(hpt_hook);

/*
 * Timer callback function
 */
static enum hrtimer_restart hpt_callback(struct hrtimer *timer)
{
	hp_current_time = ktime_add(hp_current_time, hp_delta);
	hrtimer_start(&hp_timer, hp_delta, HRTIMER_MODE_REL);

#ifdef DEBUG_HPT
	struct timeval tm = ktime_to_timeval(hp_current_time);
	/*
	 * Using RTCs we can achieve at most 1KHz frequency
	 */
	printk(KERN_INFO
		"%s: time %u sec - %u usec"
		" [%llu ns]"
		" [jiffies %d]\n",
		__func__,
		tm.tv_sec,
		tm.tv_usec,
		timeval_to_ns(&tm),
		jiffies_to_msecs(jiffies));
#endif
	
	return HRTIMER_NORESTART;
}

static int __init hpt_init(void)
{
	printk(KERN_ALERT
		"%s: Loading hpt module 1.0",
		__func__);
	
	hp_delta = ktime_set(0, US_TO_NS((unsigned long)hp_delta_us));
	/* Initialize the timer */
	hrtimer_init(&hp_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hp_timer.function = &hpt_callback;

	/* Fire the timer */
	hrtimer_start(&hp_timer, hp_delta, HRTIMER_MODE_REL);

	return 0;
}

static void __exit hpt_exit(void)
{
	int ret = 0;

	ret = hrtimer_cancel(&hp_timer);
	if (ret)
		printk(KERN_ERR
			"%s: the timer is still in use ..."
			" just wait the callback\n",
			__func__);

	printk(KERN_ALERT
		"%s: Unloading hpt module 1.0\n",
		__func__);
}

module_init(hpt_init);
module_exit(hpt_exit);
