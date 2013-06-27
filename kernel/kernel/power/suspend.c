/*
 * kernel/power/suspend.c - Suspend to RAM and standby functionality.
 *
 * Copyright (c) 2003 Patrick Mochel
 * Copyright (c) 2003 Open Source Development Lab
 * Copyright (c) 2009 Rafael J. Wysocki <rjw@sisk.pl>, Novell Inc.
 *
 * This file is released under the GPLv2.
 */

#include <linux/string.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/cpu.h>
#include <linux/syscalls.h>
#include <linux/gfp.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <trace/events/power.h>

#include "power.h"

const char *const pm_states[PM_SUSPEND_MAX] = {
#ifdef CONFIG_EARLYSUSPEND
	[PM_SUSPEND_ON]		= "on",
#endif
	[PM_SUSPEND_STANDBY]	= "standby",
	[PM_SUSPEND_MEM]	= "mem",
};

static const struct platform_suspend_ops *suspend_ops;

/**
 *	suspend_set_ops - Set the global suspend method table.
 *	@ops:	Pointer to ops structure.
 */
void suspend_set_ops(const struct platform_suspend_ops *ops)
{
	mutex_lock(&pm_mutex);
	suspend_ops = ops;
	mutex_unlock(&pm_mutex);
}

bool valid_state(suspend_state_t state)
{
	/*
	 * All states need lowlevel support and need to be valid to the lowlevel
	 * implementation, no valid callback implies that none are valid.
	 */
	return suspend_ops && suspend_ops->valid && suspend_ops->valid(state);
}

/**
 * suspend_valid_only_mem - generic memory-only valid callback
 *
 * Platform drivers that implement mem suspend only and only need
 * to check for that in their .valid callback can use this instead
 * of rolling their own .valid callback.
 */
int suspend_valid_only_mem(suspend_state_t state)
{
	return state == PM_SUSPEND_MEM;
}

static int suspend_test(int level)
{
#ifdef CONFIG_PM_DEBUG
	if (pm_test_level == level) {
		printk(KERN_INFO "suspend debug: Waiting for 5 seconds.\n");
		mdelay(5000);
		return 1;
	}
#endif /* !CONFIG_PM_DEBUG */
	return 0;
}

/**
 *	suspend_prepare - Do prep work before entering low-power state.
 *
 *	This is common code that is called for each state that we're entering.
 *	Run suspend notifiers, allocate a console and stop all processes.
 */
static int suspend_prepare(void)
{
	int error;

	if (!suspend_ops || !suspend_ops->enter)
		return -EPERM;

	pm_prepare_console();

	error = pm_notifier_call_chain(PM_SUSPEND_PREPARE);
	if (error)
		goto Finish;

	error = usermodehelper_disable();
	if (error)
		goto Finish;

	error = suspend_freeze_processes();
	if (!error)
		return 0;

	suspend_thaw_processes();
	usermodehelper_enable();
 Finish:
	pm_notifier_call_chain(PM_POST_SUSPEND);
	pm_restore_console();
	return error;
}

/* default implementation */
void __attribute__ ((weak)) arch_suspend_disable_irqs(void)
{
	local_irq_disable();
}

/* default implementation */
void __attribute__ ((weak)) arch_suspend_enable_irqs(void)
{
	local_irq_enable();
}

/**
 *	suspend_enter - enter the desired system sleep state.
 *	@state:		state to enter
 *
 *	This function should be called after devices have been suspended.
 */
static int suspend_enter(suspend_state_t state)
{
	int error;

	if (suspend_ops->prepare) {
		error = suspend_ops->prepare();
		if (error)
			goto Platform_finish;
	}

	error = dpm_suspend_noirq(PMSG_SUSPEND);
	if (error) {
		printk(KERN_ERR "PM: Some devices failed to power down\n");
		goto Platform_finish;
	}

	if (suspend_ops->prepare_late) {
		error = suspend_ops->prepare_late();
		if (error)
			goto Platform_wake;
	}

	if (suspend_test(TEST_PLATFORM))
		goto Platform_wake;

	error = disable_nonboot_cpus();
	if (error || suspend_test(TEST_CPUS))
		goto Enable_cpus;

	arch_suspend_disable_irqs();
	BUG_ON(!irqs_disabled());

	suspend_test_start_level(TEST_SUSPEND_TIME_LEVEL2);
	error = syscore_suspend();
	suspend_test_finish_level(TEST_SUSPEND_TIME_LEVEL2, "syscore_suspend");
	if (!error) {
		suspend_test_finish_level(TEST_SUSPEND_TIME_TOP, "total transition time of suspend");
		if (!(suspend_test(TEST_CORE) || pm_wakeup_pending())) {
			error = suspend_ops->enter(state);
			events_check_enabled = false;
		}
		suspend_test_start_level(TEST_SUSPEND_TIME_TOP);
		suspend_test_start_level(TEST_SUSPEND_TIME_LEVEL2);
		syscore_resume();
		suspend_test_finish_level(TEST_SUSPEND_TIME_LEVEL2, "syscore_resume");
	}

	arch_suspend_enable_irqs();
	BUG_ON(irqs_disabled());

 Enable_cpus:
	enable_nonboot_cpus();

 Platform_wake:
	if (suspend_ops->wake)
		suspend_ops->wake();

	dpm_resume_noirq(PMSG_RESUME);

 Platform_finish:
	if (suspend_ops->finish)
		suspend_ops->finish();

	return error;
}

/**
 *	suspend_devices_and_enter - suspend devices and enter the desired system
 *				    sleep state.
 *	@state:		  state to enter
 */
int suspend_devices_and_enter(suspend_state_t state)
{
	int error;
	gfp_t saved_mask;

	if (!suspend_ops)
		return -ENOSYS;

	trace_machine_suspend(state);
	if (suspend_ops->begin) {
		error = suspend_ops->begin(state);
		if (error)
			goto Close;
	}
	suspend_console();
//	saved_mask = clear_gfp_allowed_mask(GFP_IOFS);
	suspend_test_start();
	error = dpm_suspend_start(PMSG_SUSPEND);
	if (error) {
		printk(KERN_ERR "PM: Some devices failed to suspend\n");
		goto Recover_platform;
	}
	suspend_test_finish("suspend devices");
	if (suspend_test(TEST_DEVICES))
		goto Recover_platform;

	error = suspend_enter(state);

 Resume_devices:
	suspend_test_start();
	dpm_resume_end(PMSG_RESUME);
	suspend_test_finish("resume devices");
//	set_gfp_allowed_mask(saved_mask);
	resume_console();
 Close:
	if (suspend_ops->end)
		suspend_ops->end();
	trace_machine_suspend(PWR_EVENT_EXIT);
	return error;

 Recover_platform:
	if (suspend_ops->recover)
		suspend_ops->recover();
	goto Resume_devices;
}

/**
 *	suspend_finish - Do final work before exiting suspend sequence.
 *
 *	Call platform code to clean up, restart processes, and free the
 *	console that we've allocated. This is not called for suspend-to-disk.
 */
static void suspend_finish(void)
{
	suspend_thaw_processes();
	usermodehelper_enable();
	pm_notifier_call_chain(PM_POST_SUSPEND);
	pm_restore_console();
}

/**
 *	enter_state - Do common work of entering low-power state.
 *	@state:		pm_state structure for state we're entering.
 *
 *	Make sure we're the only ones trying to enter a sleep state. Fail
 *	if someone has beat us to it, since we don't want anything weird to
 *	happen when we wake up.
 *	Then, do the setup for suspend, enter the state, and cleaup (after
 *	we've woken up).
 */
#if defined(CONFIG_USB_EHCI_HCD_MODULE)
extern int tcc_umh_ehci_module(int flag);
#endif

#if defined(CONFIG_BROADCOM_WIFI)
int tcc_bcmwifi_module(int flag);
#endif

static unsigned long g_rel_cpu_clock = 0;	//for suspend.resume lock up issue
int enter_state(suspend_state_t state)
{
	int error;

	//Start : for suspend.resume lock up issue
	unsigned long n_rel_cpu_clock = (cpu_clock(smp_processor_id()) >> 30LL) - g_rel_cpu_clock;
	if(n_rel_cpu_clock < 3)
	{
		//printk("%s(%d->%d)\n", __func__, g_rel_cpu_clock, n_rel_cpu_clock);
		msleep(1000);
	}
	//End

	suspend_test_start_level(TEST_SUSPEND_TIME_TOP);

#if defined(CONFIG_USB_EHCI_HCD_MODULE)
	tcc_umh_ehci_module(0);
#endif
#if defined(CONFIG_BROADCOM_WIFI)
	int ret = tcc_bcmwifi_module(0);
#endif

	if (!valid_state(state)) {
		error = -ENODEV;
		goto End;
	}

#if defined(CONFIG_USB_EHCI_HCD_MODULE)
    tcc_umh_ehci_module(0);
#endif

	if (!mutex_trylock(&pm_mutex)) {
		error = -EBUSY;
		goto End;
	}

	printk(KERN_INFO "PM: Syncing filesystems ... ");
	sys_sync();
	printk("done.\n");

	pr_debug("PM: Preparing system for %s sleep\n", pm_states[state]);
	error = suspend_prepare();
	if (error)
		goto Unlock;

	if (suspend_test(TEST_FREEZER))
		goto Finish;

	pr_debug("PM: Entering %s sleep\n", pm_states[state]);
	pm_restrict_gfp_mask();
	error = suspend_devices_and_enter(state);
	pm_restore_gfp_mask();

 Finish:
	pr_debug("PM: Finishing wakeup.\n");
	suspend_finish();
 Unlock:
	mutex_unlock(&pm_mutex);

 End:
#if defined(CONFIG_USB_EHCI_HCD_MODULE)
	tcc_umh_ehci_module(1);
#endif

#if defined(CONFIG_BROADCOM_WIFI)
	if(ret==0)
		tcc_bcmwifi_module(1);
#endif

	suspend_test_finish_level(TEST_SUSPEND_TIME_TOP, "total transition time of resume");

	//Start : for suspend.resume lock up issue
	g_rel_cpu_clock = (cpu_clock(smp_processor_id()) >> 30LL);
	//End

	return error;
}

/**
 *	pm_suspend - Externally visible function for suspending system.
 *	@state:		Enumerated value of state to enter.
 *
 *	Determine whether or not value is within range, get state
 *	structure, and enter (above).
 */
int pm_suspend(suspend_state_t state)
{
	if (state > PM_SUSPEND_ON && state <= PM_SUSPEND_MAX)
		return enter_state(state);
	return -EINVAL;
}
EXPORT_SYMBOL(pm_suspend);

#if defined(CONFIG_MACH_M805_892X) && defined(CONFIG_WIFI_PWR_CTL)
#include <linux/gpio.h>
extern wifi_stat;
#endif
#if defined(CONFIG_USB_EHCI_HCD_MODULE)
int tcc_umh_ehci_module(int flag)
{
	int retval = 0;

/* Only considered for TCC88xx */
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	struct subprocess_info *sub_info_hs, *sub_info_fs;
	static char *envp[] = {NULL};
	char *argv_hs[] = {
		flag ? "/system/bin/insmod" : "/system/bin/rmmod",
		flag ? "/lib/modules/ehci-hcd.ko" : "ehci_hcd",
		NULL};
	char *argv_fs[] = {
		flag ? "/system/bin/insmod" : "/system/bin/rmmod",
		flag ? "/lib/modules/ohci-hcd.ko" : "ohci_hcd",
		NULL};

	sub_info_hs = call_usermodehelper_setup( argv_hs[0], argv_hs, envp, GFP_ATOMIC );
	sub_info_fs = call_usermodehelper_setup( argv_fs[0], argv_fs, envp, GFP_ATOMIC );
	if (sub_info_hs == NULL || sub_info_fs == NULL) {
	printk("-> [%s:%d] ERROR-hs:0x%p, fs:0x%p\n", __func__, __LINE__, sub_info_hs, sub_info_fs);
		if(sub_info_hs) call_usermodehelper_freeinfo(sub_info_hs);
		if(sub_info_fs) call_usermodehelper_freeinfo(sub_info_fs);
		retval = -ENOMEM;
		goto End;
	}

	if (flag) {
#if defined(CONFIG_MACH_M805_892X) && defined(CONFIG_WIFI_PWR_CTL)
		if(wifi_stat==1)
			gpio_direction_output(TCC_GPE(3), 1);
#endif
		retval = call_usermodehelper_exec(sub_info_hs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
		retval |= call_usermodehelper_exec(sub_info_fs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
	} else {
		retval = call_usermodehelper_exec(sub_info_fs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
		retval |= call_usermodehelper_exec(sub_info_hs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
#if defined(CONFIG_MACH_M805_892X) && defined(CONFIG_WIFI_PWR_CTL)
		gpio_direction_output(TCC_GPE(3), 0);
#endif
	}
#endif /* CONFIG_ARCH_TCC88XX */

End:
	return retval;
}
EXPORT_SYMBOL(tcc_umh_ehci_module);
#endif

#if defined(CONFIG_BROADCOM_WIFI)
int tcc_bcmwifi_module(int flag)
{
	int retval = 0;
	struct subprocess_info *sub_info_hs;
	static char *envp[] = {NULL};
	char *argv_hs[] = {
		flag ? "/system/bin/insmod" : "/system/bin/rmmod",
		flag ? "/system/wifi/bcm4330.ko" : "bcmdhd",
		NULL};

	sub_info_hs = call_usermodehelper_setup( argv_hs[0], argv_hs, envp, GFP_ATOMIC );
	if (sub_info_hs == NULL) {
	printk("-> [%s:%d] ERROR-hs:0x%p", __func__, __LINE__, sub_info_hs);
		if(sub_info_hs) call_usermodehelper_freeinfo(sub_info_hs);
		retval = -ENOMEM;
		goto End;
	}

	if (flag) {
		retval = call_usermodehelper_exec(sub_info_hs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
	} else {
		retval |= call_usermodehelper_exec(sub_info_hs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
	}
End:
	return retval;
}
EXPORT_SYMBOL(tcc_bcmwifi_module);
#endif
