/*
 * kernel/power/hibernate.c - Hibernation (a.k.a suspend-to-disk) support.
 *
 * Copyright (c) 2003 Patrick Mochel
 * Copyright (c) 2003 Open Source Development Lab
 * Copyright (c) 2004 Pavel Machek <pavel@ucw.cz>
 * Copyright (c) 2009 Rafael J. Wysocki, Novell Inc.
 *
 * This file is released under the GPLv2.
 */

#include <linux/suspend.h>
#include <linux/syscalls.h>
#include <linux/reboot.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/kmod.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/pm.h>
#include <linux/console.h>
#include <linux/cpu.h>
#include <linux/freezer.h>
#include <linux/gfp.h>
#include <linux/syscore_ops.h>
#include <scsi/scsi_scan.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/clk.h>

#include "power.h"
#include <mach/globals.h>
#include <mach/io.h>
#include <linux/platform_device.h>
#include <mach/reg_physical.h>
#include <linux/mm.h>

//extern volatile PLCDC_CHANNEL pLCDC_FB_CH;

#if defined(CONFIG_USB_EHCI_HCD_MODULE)
extern int tcc_umh_ehci_module(int flag);
#endif

extern void hibernate_thaw_processes(void);

#ifdef CONFIG_SNAPSHOT_BOOT
#ifdef CONFIG_NOCOMPRESS_SNAPSHOT
static int nocompress = 1;
#else
static int nocompress = 0;
#endif
#else
static int nocompress = 0;
#endif
//B090162==
static int noresume = 0;
static char resume_file[256] = CONFIG_PM_STD_PARTITION;
dev_t swsusp_resume_device;
sector_t swsusp_resume_block;
//B090162==
int in_suspend __nosavedata = 0;
extern unsigned int do_hibernation;
unsigned int do_yaffs_remount = 0;

unsigned int do_hibernate_boot = 0;
EXPORT_SYMBOL(do_hibernate_boot);

unsigned long long snapshot_restore_start;
unsigned long long quickboot_profile_core_resume;
unsigned long long profile_dpm_resume;

enum {
	HIBERNATION_INVALID,
	HIBERNATION_PLATFORM,
	HIBERNATION_TEST,
	HIBERNATION_TESTPROC,
	HIBERNATION_SHUTDOWN,
	HIBERNATION_REBOOT,
	/* keep last */
	__HIBERNATION_AFTER_LAST
};
#define HIBERNATION_MAX (__HIBERNATION_AFTER_LAST-1)
#define HIBERNATION_FIRST (HIBERNATION_INVALID + 1)

static int hibernation_mode = HIBERNATION_REBOOT;

static const struct platform_hibernation_ops *hibernation_ops;
//B090162
int load_tcc_init_nand_module(void);

//===================================================================================
static void kill_for_name(char *name) 
{
    struct task_struct *p;

    read_lock(&tasklist_lock);
    for_each_process(p) {

        //printk("[%s] processl [%s : %s] \n", __FUNCTION__, p->comm, name);
    
        if( !strcmp(name, p->comm) ) {
            read_unlock(&tasklist_lock);
            
            printk("[%s] send kill signal [%s : %d] \n", __FUNCTION__, p->comm, p->pid);
            send_sig(SIGKILL, p, 0);
            return;
        }
    }
    read_unlock(&tasklist_lock);

    return;
}

static struct task_struct* getTaskStackForName(char *name) 
{
    struct task_struct *p;

    read_lock(&tasklist_lock);
    for_each_process(p) {

        printk("[%s] processl [%s : %s] \n", __FUNCTION__, p->comm, name);
    
        if( !strcmp(name, p->comm) ) {
            read_unlock(&tasklist_lock);
            
            printk("[%s] find pid [%s : %d] \n", __FUNCTION__, p->comm, p->pid);
            read_unlock(&tasklist_lock);
            return p;
        }
    }
    read_unlock(&tasklist_lock);

    return NULL;
}



/**
 * hibernation_set_ops - Set the global hibernate operations.
 * @ops: Hibernation operations to use in subsequent hibernation transitions.
 */
void hibernation_set_ops(const struct platform_hibernation_ops *ops)
{
	if (ops && !(ops->begin && ops->end &&  ops->pre_snapshot
	    && ops->prepare && ops->finish && ops->enter && ops->pre_restore
	    && ops->restore_cleanup && ops->leave)) {
		WARN_ON(1);
		return;
	}
	mutex_lock(&pm_mutex);
	hibernation_ops = ops;
	if (ops)
		hibernation_mode = HIBERNATION_PLATFORM;
	else if (hibernation_mode == HIBERNATION_PLATFORM)
		hibernation_mode = HIBERNATION_REBOOT;

	mutex_unlock(&pm_mutex);
}

static bool entering_platform_hibernation;

bool system_entering_hibernation(void)
{
	return entering_platform_hibernation;
}
EXPORT_SYMBOL(system_entering_hibernation);

#ifdef CONFIG_PM_DEBUG
static void hibernation_debug_sleep(void)
{
	printk(KERN_INFO "hibernation debug: Waiting for 5 seconds.\n");
	mdelay(5000);
}

static int hibernation_testmode(int mode)
{
	if (hibernation_mode == mode) {
		hibernation_debug_sleep();
		return 1;
	}
	return 0;
}

static int hibernation_test(int level)
{
	if (pm_test_level == level) {
		hibernation_debug_sleep();
		return 1;
	}
	return 0;
}
#else /* !CONFIG_PM_DEBUG */
static int hibernation_testmode(int mode) { return 0; }
static int hibernation_test(int level) { return 0; }
#endif /* !CONFIG_PM_DEBUG */

/**
 * platform_begin - Call platform to start hibernation.
 * @platform_mode: Whether or not to use the platform driver.
 */
static int platform_begin(int platform_mode)
{
	return (platform_mode && hibernation_ops) ?
		hibernation_ops->begin() : 0;
}

/**
 * platform_end - Call platform to finish transition to the working state.
 * @platform_mode: Whether or not to use the platform driver.
 */
static void platform_end(int platform_mode)
{
	if (platform_mode && hibernation_ops)
		hibernation_ops->end();
}

/**
 * platform_pre_snapshot - Call platform to prepare the machine for hibernation.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Use the platform driver to prepare the system for creating a hibernate image,
 * if so configured, and return an error code if that fails.
 */

static int platform_pre_snapshot(int platform_mode)
{
	return (platform_mode && hibernation_ops) ?
		hibernation_ops->pre_snapshot() : 0;
}

/**
 * platform_leave - Call platform to prepare a transition to the working state.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Use the platform driver prepare to prepare the machine for switching to the
 * normal mode of operation.
 *
 * This routine is called on one CPU with interrupts disabled.
 */
static void platform_leave(int platform_mode)
{
	if (platform_mode && hibernation_ops)
		hibernation_ops->leave();
}

/**
 * platform_finish - Call platform to switch the system to the working state.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Use the platform driver to switch the machine to the normal mode of
 * operation.
 *
 * This routine must be called after platform_prepare().
 */
static void platform_finish(int platform_mode)
{
	if (platform_mode && hibernation_ops)
		hibernation_ops->finish();
}

/**
 * platform_pre_restore - Prepare for hibernate image restoration.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Use the platform driver to prepare the system for resume from a hibernation
 * image.
 *
 * If the restore fails after this function has been called,
 * platform_restore_cleanup() must be called.
 */
static int platform_pre_restore(int platform_mode)
{
	return (platform_mode && hibernation_ops) ?
		hibernation_ops->pre_restore() : 0;
}

/**
 * platform_restore_cleanup - Switch to the working state after failing restore.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Use the platform driver to switch the system to the normal mode of operation
 * after a failing restore.
 *
 * If platform_pre_restore() has been called before the failing restore, this
 * function must be called too, regardless of the result of
 * platform_pre_restore().
 */
static void platform_restore_cleanup(int platform_mode)
{
	if (platform_mode && hibernation_ops)
		hibernation_ops->restore_cleanup();
}

/**
 * platform_recover - Recover from a failure to suspend devices.
 * @platform_mode: Whether or not to use the platform driver.
 */
static void platform_recover(int platform_mode)
{
	if (platform_mode && hibernation_ops && hibernation_ops->recover)
		hibernation_ops->recover();
}

/**
 * swsusp_show_speed - Print time elapsed between two events during hibernation.
 * @start: Starting event.
 * @stop: Final event.
 * @nr_pages: Number of memory pages processed between @start and @stop.
 * @msg: Additional diagnostic message to print.
 */
void swsusp_show_speed(struct timeval *start, struct timeval *stop,
			unsigned nr_pages, char *msg)
{
	s64 elapsed_centisecs64;
	int centisecs;
	int k;
	int kps;

	elapsed_centisecs64 = timeval_to_ns(stop) - timeval_to_ns(start);
	do_div(elapsed_centisecs64, NSEC_PER_SEC / 100);
	centisecs = elapsed_centisecs64;
	if (centisecs == 0)
		centisecs = 1;	/* avoid div-by-zero */
	k = nr_pages * (PAGE_SIZE / 1024);
	kps = (k * 100) / centisecs;
	printk(KERN_INFO "PM: %s %d kbytes in %d.%02d seconds (%d.%02d MB/s)\n",
			msg, k,
			centisecs / 100, centisecs % 100,
			kps / 1000, (kps % 1000) / 10);
}


/**
 *	swsusp_show_speed - print the time elapsed between two events.
 *	@start: Starting event.
 *	@stop: Final event.
 *	@nr_pages -	number of pages processed between @start and @stop
 *	@msg -		introductory message to print
 */

void swsusp_show_speed1(s64 elapsed_centisecs64,
			unsigned nr_pages, char *msg)
{
	int centisecs;
	int k;
	int kps;

	do_div(elapsed_centisecs64, NSEC_PER_SEC / 100);
	centisecs = elapsed_centisecs64;
	if (centisecs == 0)
		centisecs = 1;	/* avoid div-by-zero */
	k = nr_pages * (PAGE_SIZE / 1024);
	kps = (k * 100) / centisecs;
	printk(KERN_INFO "PM: %s %d kbytes in %d.%02d seconds (%d.%02d MB/s)\n",
			msg, k,
			centisecs / 100, centisecs % 100,
			kps / 1000, (kps % 1000) / 10);
}

/**
 * create_image - Create a hibernation image.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Execute device drivers' .freeze_noirq() callbacks, create a hibernation image
 * and execute the drivers' .thaw_noirq() callbacks.
 *
 * Control reappears in this routine after the subsequent restore.
 */
#ifdef CONFIG_SNAPSHOT_BOOT
/*===========================================================================
for snapshot boot
==============================================================================*/
extern void snapshot_mmu_switching(void);
extern void snapshot_state_store(void);
extern void snapshot_state_restore(void);
unsigned int reg[100]; 
extern unsigned long do_snapshot_boot;
EXPORT_SYMBOL(do_snapshot_boot);
extern void restore_snapshot_cpu_reg(void);
extern int save_cpu_reg_for_snapshot(unsigned int ptable, unsigned int pReg, void *);
#endif
static int create_image(int platform_mode)
{
	int error;
	unsigned  long long t;
#ifdef CONFIG_SNAPSHOT_BOOT
	static PPMU pPMU;
	pPMU = (PPMU)tcc_p2v(HwPMU_BASE);
#endif
	error = dpm_suspend_noirq(PMSG_FREEZE);
	if (error) {
		printk(KERN_ERR "PM: Some devices failed to power down, "
			"aborting hibernation\n");
		return error;
	}

	error = platform_pre_snapshot(platform_mode);
	if (error || hibernation_test(TEST_PLATFORM))
		goto Platform_finish;

	error = disable_nonboot_cpus();
	if (error || hibernation_test(TEST_CPUS)
	    || hibernation_testmode(HIBERNATION_TEST))
		goto Enable_cpus;

	local_irq_disable();

	error = syscore_suspend();
	if (error) {
		printk(KERN_ERR "PM: Some system devices failed to power down, "
			"aborting hibernation\n");
		goto Enable_irqs;
	}
#ifdef CONFIG_SNAPSHOT_BOOT
	do_snapshot_boot = 0;
#endif
	if (hibernation_test(TEST_CORE) || pm_wakeup_pending())
		goto Power_up;

	in_suspend = 1;
#ifdef CONFIG_ARCH_TCC892X
	save_processor_a5_state();
#else
	save_processor_state();
#endif
#ifdef CONFIG_SNAPSHOT_BOOT
	/*===========================================================================
	Save context for snapshot boot
	===========================================================================*/
	snapshot_state_store();

	save_cpu_reg_for_snapshot(0x0, reg, restore_snapshot_cpu_reg);	
#else	
	error = swsusp_arch_suspend();
#endif
	if (error)
		printk(KERN_ERR "PM: Error %d creating hibernation image\n",
			error);
	/* Restore control flow magically appears here */

	snapshot_restore_start = cpu_clock(smp_processor_id());

#ifdef CONFIG_SNAPSHOT_BOOT
	if (do_snapshot_boot) {
		/*===========================================================================
		Restore Register status.
		===========================================================================*/
		snapshot_state_restore();
		//touch_softlockup_watchdog();
		in_suspend = 0;
#if 0
#ifdef CONFIG_ARCH_TCC892X
      pPMU->PMU_WDTCTRL.bREG.CLR = 1;
#elif CONFIG_ARCH_TCC88XX
      BITSET(pPMU->WATCHDOG, Hw30);
#endif
#endif
	}
#endif
#ifdef CONFIG_ARCH_TCC892X
	restore_processor_a5_state();
#else
	restore_processor_state();
#endif
	if (!in_suspend) {
		do_hibernate_boot = 1;
		events_check_enabled = false;
		platform_leave(platform_mode);
	}

 Power_up:
	syscore_resume();

 Enable_irqs:
	local_irq_enable();

 Enable_cpus:
	enable_nonboot_cpus();

 Platform_finish:
	platform_finish(platform_mode);

    #ifdef CONFIG_SNAPSHOT_BOOT
	dpm_hibernate_uart_resume(PMSG_RESTORE);
	resume_hibernate_console();
    #endif

	dpm_resume_noirq(in_suspend ?
		(error ? PMSG_RECOVER : PMSG_THAW) : PMSG_RESTORE);

	return error;
}

/**
 * hibernation_snapshot - Quiesce devices and create a hibernation image.
 * @platform_mode: If set, use platform driver to prepare for the transition.
 *
 * This routine must be called with pm_mutex held.
 */
int hibernation_snapshot(int platform_mode)
{
	pm_message_t msg = PMSG_RECOVER;
	int error;
#ifdef CONFIG_SNAPSHOT_BOOT
	static PPMU pPMU;
	pPMU = (PPMU)tcc_p2v(HwPMU_BASE);
#endif
	unsigned long long t;
	
	error = platform_begin(platform_mode);
	if (error)
		goto Close;

	error = dpm_prepare(PMSG_FREEZE);
	if (error)
		goto Complete_devices;

	/* Preallocate image memory before shutting down devices. */
	error = hibernate_preallocate_memory();
	if (error)
		goto Complete_devices;

	suspend_console();
	pm_restrict_gfp_mask();
	error = dpm_suspend(PMSG_FREEZE);
	if (error)
		goto Recover_platform;

	if (hibernation_test(TEST_DEVICES))
		goto Recover_platform;

	error = create_image(platform_mode);
	/*
	 * Control returns here (1) after the image has been created or the
	 * image creation has failed and (2) after a successful restore.
	 */

 Resume_devices:
	/* We may need to release the preallocated image pages here. */
	if (error || !in_suspend)
		swsusp_free();

	msg = in_suspend ? (error ? PMSG_RECOVER : PMSG_THAW) : PMSG_RESTORE;
   
#if 0
#ifdef CONFIG_SNAPSHOT_BOOT
   if (do_snapshot_boot) {
    #ifdef CONFIG_ARCH_TCC892X
      pPMU->PMU_WDTCTRL.bREG.CLR = 1;
    #elif CONFIG_ARCH_TCC88XX
      BITSET(pPMU->WATCHDOG, Hw30);
    #endif
   }
#endif
#endif
	dpm_resume(msg);

#if 0
#ifdef CONFIG_SNAPSHOT_BOOT
   if (do_snapshot_boot) {
    #ifdef CONFIG_ARCH_TCC892X
      pPMU->PMU_WDTCTRL.bREG.CLR = 1;
    #elif CONFIG_ARCH_TCC88XX
      BITSET(pPMU->WATCHDOG, Hw30);
    #endif
   }
#endif
#endif

	t = cpu_clock(smp_processor_id());
	
	dpm_resume(msg);

	profile_dpm_resume = (cpu_clock(smp_processor_id()) - t);
	
	if (error || !in_suspend)
		pm_restore_gfp_mask();

#ifndef CONFIG_SNAPSHOT_BOOT
	resume_console();
#endif

 Complete_devices:
	dpm_complete(msg);

 Close:
	platform_end(platform_mode);
	return error;

 Recover_platform:
	platform_recover(platform_mode);
	goto Resume_devices;
}

/**
 * resume_target_kernel - Restore system state from a hibernation image.
 * @platform_mode: Whether or not to use the platform driver.
 *
 * Execute device drivers' .freeze_noirq() callbacks, restore the contents of
 * highmem that have not been restored yet from the image and run the low-level
 * code that will restore the remaining contents of memory and switch to the
 * just restored target kernel.
 */
static int resume_target_kernel(bool platform_mode)
{
	int error;

	error = dpm_suspend_noirq(PMSG_QUIESCE);
	if (error) {
		printk(KERN_ERR "PM: Some devices failed to power down, "
			"aborting resume\n");
		return error;
	}

	error = platform_pre_restore(platform_mode);
	if (error)
		goto Cleanup;

	error = disable_nonboot_cpus();
	if (error)
		goto Enable_cpus;

	local_irq_disable();

	error = syscore_suspend();
	if (error)
		goto Enable_irqs;

	//BITCSET(pLCDC_FB_CH->LIC,0x30000000,0x00000000);
	//pLCDC_FB_CH->LIC |= (HwLCT_RU);	// fb disable
#ifdef CONFIG_ARCH_TCC892X
	save_processor_a5_state();
#else	
	save_processor_state();
#endif
	error = restore_highmem();
	if (!error) {
		error = swsusp_arch_resume();
		/*
		 * The code below is only ever reached in case of a failure.
		 * Otherwise, execution continues at the place where
		 * swsusp_arch_suspend() was called.
		 */
		BUG_ON(!error);
		/*
		 * This call to restore_highmem() reverts the changes made by
		 * the previous one.
		 */
		restore_highmem();
	}
	/*
	 * The only reason why swsusp_arch_resume() can fail is memory being
	 * very tight, so we have to free it as soon as we can to avoid
	 * subsequent failures.
	 */
	swsusp_free();
#ifdef CONFIG_ARCH_TCC892X	
	restore_processor_a5_state();
#else
	restore_processor_state();
#endif
	touch_softlockup_watchdog();

	syscore_resume();

 Enable_irqs:
	local_irq_enable();

 Enable_cpus:
	enable_nonboot_cpus();

 Cleanup:
	platform_restore_cleanup(platform_mode);

	dpm_resume_noirq(PMSG_RECOVER);

	return error;
}

/**
 * hibernation_restore - Quiesce devices and restore from a hibernation image.
 * @platform_mode: If set, use platform driver to prepare for the transition.
 *
 * This routine must be called with pm_mutex held.  If it is successful, control
 * reappears in the restored target kernel in hibernation_snaphot().
 */
int hibernation_restore(int platform_mode)
{
	int error;

	pm_prepare_console();
	suspend_console();
	pm_restrict_gfp_mask();
	error = dpm_suspend_start(PMSG_QUIESCE);
	if (!error) {
		error = resume_target_kernel(platform_mode);
		dpm_resume_end(PMSG_RECOVER);
	}
	pm_restore_gfp_mask();
	resume_console();
	pm_restore_console();
	return error;
}

/**
 * hibernation_platform_enter - Power off the system using the platform driver.
 */
int hibernation_platform_enter(void)
{
	int error;

	if (!hibernation_ops)
		return -ENOSYS;

	/*
	 * We have cancelled the power transition by running
	 * hibernation_ops->finish() before saving the image, so we should let
	 * the firmware know that we're going to enter the sleep state after all
	 */
	error = hibernation_ops->begin();
	if (error)
		goto Close;

	entering_platform_hibernation = true;
	suspend_console();
	error = dpm_suspend_start(PMSG_HIBERNATE);
	if (error) {
		if (hibernation_ops->recover)
			hibernation_ops->recover();
		goto Resume_devices;
	}

	error = dpm_suspend_noirq(PMSG_HIBERNATE);
	if (error)
		goto Resume_devices;

	error = hibernation_ops->prepare();
	if (error)
		goto Platform_finish;

	error = disable_nonboot_cpus();
	if (error)
		goto Platform_finish;

	local_irq_disable();
	syscore_suspend();
	if (pm_wakeup_pending()) {
		error = -EAGAIN;
		goto Power_up;
	}

	hibernation_ops->enter();
	/* We should never get here */
	while (1);

 Power_up:
	syscore_resume();
	local_irq_enable();
	enable_nonboot_cpus();

 Platform_finish:
	hibernation_ops->finish();

	dpm_resume_noirq(PMSG_RESTORE);

 Resume_devices:
	entering_platform_hibernation = false;
	dpm_resume_end(PMSG_RESTORE);
	resume_console();

 Close:
	hibernation_ops->end();

	return error;
}

/**
 * power_down - Shut the machine down for hibernation.
 *
 * Use the platform driver, if configured, to put the system into the sleep
 * state corresponding to hibernation, or try to power it off or reboot,
 * depending on the value of hibernation_mode.
 */
static void power_down(void)
{
	switch (hibernation_mode) {
	case HIBERNATION_TEST:
	case HIBERNATION_TESTPROC:
		break;
	case HIBERNATION_REBOOT:
		kernel_restart(NULL);
		break;
	case HIBERNATION_PLATFORM:
		hibernation_platform_enter();
	case HIBERNATION_SHUTDOWN:
		kernel_power_off();
		break;
	}
	kernel_halt();
	/*
	 * Valid image is on the disk, if we continue we risk serious data
	 * corruption after resume.
	 */
	printk(KERN_CRIT "PM: Please power down manually\n");
	while(1);
}

static int prepare_processes(void)
{
	int error = 0;	
	if (freeze_processes()) {
		error = -EBUSY;
		thaw_processes();
	}
	return error;
}

int set_hibernate_boot_property(void) {
	static char * path = "/system/bin/setprop";

	#ifdef CONFIG_SNAPSHOT_BOOT
	char *argv[] = { path,  "tcc.hibernate.boot", "snapshot", NULL};
	#else
	char *argv[] = { path,  "tcc.hibernate.boot", "hibernate", NULL};
	#endif
	
	static char *envp[] = { "HOME=/",
				"TERM=linux",
				"PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
				NULL };

	return call_usermodehelper(path, argv, envp, UMH_WAIT_PROC);
}

int set_hibernate_swap_path_property(void) {
	static char * path = "/system/bin/setprop";

	char *argv[] = { path,  "tcc.swap.path", resume_file, NULL};
	
	static char *envp[] = { "HOME=/",
				"TERM=linux",
				"PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
				NULL };

	return call_usermodehelper(path, argv, envp, UMH_WAIT_PROC);
}

int reload_persistent_property(void) {
    static char * path = "/system/bin/reload_persistent_property";
    char *argv[] = { path,  NULL};
    static char *envp[] = { "HOME=/",
                "TERM=linux",
                "PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
                NULL };

    return call_usermodehelper(path, argv, envp, UMH_WAIT_PROC);
}


int load_module(char* path) {
    static char * cmdpath = "/system/bin/insmod";
    char *argv[] = { cmdpath,  path, NULL};
    static char *envp[] = { "HOME=/",
                "TERM=linux",
                "PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
                NULL };

    return call_usermodehelper(cmdpath, argv, envp, UMH_WAIT_PROC);
}

int hibernate_mkswap(char* path) {
    static char * cmdpath = "/system/bin/mkswap";
    char *argv[] = { cmdpath, path, NULL};
    static char *envp[] = { "HOME=/",
                "TERM=linux",
                "PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
                NULL };

    return call_usermodehelper(cmdpath, argv, envp, UMH_WAIT_PROC);
}

int hibernate_swapon(char* path) {
    static char * cmdpath = "/system/bin/swapon";
    char *argv[] = { cmdpath, path, NULL};
    static char *envp[] = { "HOME=/",
                "TERM=linux",
                "PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
                NULL };

    return call_usermodehelper(cmdpath, argv, envp, UMH_WAIT_PROC);
}

int hibernate_swapoff(char* path) {
    static char * cmdpath = "/system/bin/swapoff";
    char *argv[] = { cmdpath, path, NULL};
    static char *envp[] = { "HOME=/",
                "TERM=linux",
                "PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
                NULL };

    return call_usermodehelper(cmdpath, argv, envp, UMH_WAIT_PROC);
}

static int slogi(char* message) {
    static char * cmdpath = "/system/bin/log";
    char *argv[] = { cmdpath, message, NULL};
    static char *envp[] = { "HOME=/",
                "TERM=linux",
                "PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
                NULL };

    return call_usermodehelper(cmdpath, argv, envp, UMH_WAIT_PROC);
}

int load_tcc_nand_module(int flag)
{
	int retval = 0;

/* Only considered for TCC88xx */
#if defined(CONFIG_ARCH_TCC88XX)
	struct subprocess_info *sub_info_hs, *sub_info_fs;
	static char *envp[] = {NULL};
	char *argv_hs[] = {
		flag ? "/system/bin/insmod" : "/system/bin/rmmod", 
		flag ? "/lib/modules/tcc_ndd.ko" : "tcc_ndd", 
		NULL};
	char *argv_fs[] = {
		flag ? "/system/bin/insmod" : "/system/bin/rmmod", 
		flag ? "/lib/modules/tcc_ndd.ko" : "tcc_ndd", 
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
		retval = call_usermodehelper_exec(sub_info_hs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
		retval |= call_usermodehelper_exec(sub_info_fs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
	} else {
		retval = call_usermodehelper_exec(sub_info_fs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
		retval |= call_usermodehelper_exec(sub_info_hs, UMH_WAIT_PROC);
		if(retval) printk("-> [%s:%d] retval:%d\n", __func__, __LINE__, retval);
	}
#endif /* CONFIG_ARCH_TCC88XX */

End:
	return retval;
}

#include <linux/rtc.h>
extern int rtc_hctosys_ret;

static int __init rtc_hctosys(void)
{
    int err = -ENODEV;
    struct rtc_time tm;
    struct timespec tv = {
        .tv_nsec = NSEC_PER_SEC >> 1,
    };
    struct rtc_device *rtc = rtc_class_open(CONFIG_RTC_HCTOSYS_DEVICE);

    if (rtc == NULL) {
        pr_err("%s: unable to open rtc device (%s)\n",
            __FILE__, CONFIG_RTC_HCTOSYS_DEVICE);
        goto err_open;
    }

    err = rtc_read_time(rtc, &tm);
    if (err) {
        dev_err(rtc->dev.parent,
            "hctosys: unable to read the hardware clock\n");
        goto err_read;

    }

    err = rtc_valid_tm(&tm);
    if (err) {
        dev_err(rtc->dev.parent,
            "hctosys: invalid date/time\n");
        goto err_invalid;
    }

    rtc_tm_to_time(&tm, &tv.tv_sec);

    do_settimeofday(&tv);

    dev_info(rtc->dev.parent,
        "setting system clock to "
        "%d-%02d-%02d %02d:%02d:%02d UTC (%u)\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec,
        (unsigned int) tv.tv_sec);

err_invalid:
err_read:
    rtc_class_close(rtc);

err_open:
    rtc_hctosys_ret = err;

    return err;
}

/**
 * hibernate - Carry out system hibernation, including saving the image.
 */
extern int fb_external_output_update(void);

#ifdef CONFIG_FB_QUICKBOOT_LOGO_FILE
extern int fb_hibernation_logo(void);
#endif

extern void tcc_usb_notification(void);
int hibernate(void)
{
	int error;
    unsigned long long start;
    unsigned long long start_remount;
    unsigned long long stop_remount;
    unsigned long long stop;
    unsigned long long remount_time;
    unsigned long long nand_init_time;
    unsigned long long total_time;
    unsigned long long resume_time;
    unsigned long long etc_time;
    unsigned long long remaind;

#ifdef CONFIG_SNAPSHOT_BOOT
	static PPMU pPMU;
	pPMU = (PPMU)tcc_p2v(HwPMU_BASE);
#endif
    char message[512];
	mutex_lock(&pm_mutex);
	/* The snapshot device should not be opened while we're running */
	if (!atomic_add_unless(&snapshot_device_available, -1, 0)) {
		error = -EBUSY;
		goto Unlock;
	}

	pm_prepare_console();
	error = pm_notifier_call_chain(PM_HIBERNATION_PREPARE);
	if (error)
		goto Exit;

	set_hibernate_swap_path_property();
	
    #if defined(CONFIG_USB_EHCI_HCD_MODULE)
        tcc_umh_ehci_module(0);
    #endif

	error = usermodehelper_disable();
	if (error)
		goto Exit;

	/* Allocate memory management structures */
	error = create_basic_memory_bitmaps();
	if (error)
		goto Exit;

	printk(KERN_INFO "PM: Syncing filesystems ... ");
   sys_sync();
   sys_sync();
	sys_sync();
	printk("done.\n");

	error = prepare_processes();
	if (error)
		goto Finish;

	if (hibernation_test(TEST_FREEZER))
		goto Thaw;

	if (hibernation_testmode(HIBERNATION_TESTPROC))
		goto Thaw;

#ifdef CONFIG_FB_QUICKBOOT_LOGO_FILE
	fb_hibernation_logo();
#endif

	error = hibernation_snapshot(hibernation_mode == HIBERNATION_PLATFORM);
	if (error) {
		printk("failed hibernation_snapshot! error = %d\n", error);
		goto Thaw;
	}

	if (in_suspend) {
		unsigned int flags = 0;

      usermodehelper_enable();

      error = hibernate_mkswap(resume_file);
      printk("hibernate_mkswap(%s) %d\n", resume_file, error);


      error = hibernate_swapon(resume_file);
      printk("hibernate_swapon(%s) %d\n", resume_file, error);

		if (hibernation_mode == HIBERNATION_PLATFORM)
			flags |= SF_PLATFORM_MODE;
		if (nocompress)
			flags |= SF_NOCOMPRESS_MODE;
		pr_debug("PM: writing image.\n");
		error = swsusp_write(flags);
		swsusp_free();
		if (!error)
			power_down();
		in_suspend = 0;
		pm_restore_gfp_mask();
	} else {
#ifdef CONFIG_SNAPSHOT_BOOT
      if (do_snapshot_boot) {
         printk("*** Disable Watchdog ***\n");
    #ifdef CONFIG_ARCH_TCC892X
         pPMU->PMU_WDTCTRL.bREG.EN = 0;
    #elif CONFIG_ARCH_TCC88XX
         BITCLR(pPMU->WATCHDOG, Hw31);
    #endif
      }
#endif

      start = cpu_clock(smp_processor_id());
      do_yaffs_remount = 1;
		pr_debug("PM: Image restored successfully.\n");
      //show_swap_cache_info();
      //fb_external_output_update();
      usermodehelper_enable();

      hibernate_thaw_processes();

      start_remount = cpu_clock(smp_processor_id());
      if( !strncmp(resume_file, "/dev/block/ndd", 14) ) {
         error = do_mount("/dev/block/ndda6", "/cache", "ext4", MS_NOSUID | MS_NODEV | MS_REMOUNT, 0);
         printk("remount /dev/block/ndda6 ret[%d]======================================================\n\n", error);
         error = do_mount("/dev/block/ndda5", "/data", "ext4", MS_NOSUID | MS_NODEV | MS_REMOUNT | MS_KERNMOUNT, 0);
         printk("remount /dev/block/ndda5 ret[%d]======================================================\n\n", error);
      } else if( !strncmp(resume_file, "/dev/block/mmc", 14) ) {
         error = do_mount("/dev/block/mmcblk0p6", "/cache", "ext4", MS_NOSUID | MS_NODEV | MS_REMOUNT, 0);
         printk("remount /dev/block/mmcblk0p6 ret[%d]======================================================\n\n", error);
         error = do_mount("/dev/block/mmcblk0p5", "/data", "ext4", MS_NOSUID | MS_NODEV | MS_REMOUNT | MS_KERNMOUNT, 0);
         printk("remount /dev/block/mmcblk0p5 ret[%d]======================================================\n\n", error);
      } else if( !strncmp(resume_file, "/dev/block/mtd", 14) ) {
         error =do_mount("/dev/block/mtdblock4", "/cache", "yaffs2", MS_NOSUID | MS_NODEV | MS_REMOUNT, 0 );
         printk("remount /dev/block/mtdblock4 ret[%d]======================================================\n\n", error);
         error =do_mount("/dev/block/mtdblock5", "/data", "yaffs2", MS_NOSUID | MS_NODEV | MS_REMOUNT, 0 );
         printk("remount /dev/block/mtdblock5 ret[%d]======================================================\n\n", error);
      }

      stop_remount = cpu_clock(smp_processor_id());
      rtc_hctosys();
      //error = reload_persistent_property();
      //printk("reload_persistent_property() ret[%d]======================================================\n\n", error);

      do_hibernation = 0;
      do_yaffs_remount = 0;

      error = set_hibernate_boot_property();
      //printk(KERN_INFO "set_hibernate_boot_property [%d] ", error);

      set_hibernate_swap_path_property();
      //printk(KERN_INFO "set_hibernate_swap_path_property [%d] ", error);
      //sys_sync();
   }

 Thaw:
	thaw_processes();
 Finish:
	free_basic_memory_bitmaps();
	//usermodehelper_enable();
 Exit:
	pm_notifier_call_chain(PM_POST_HIBERNATION);
	pm_restore_console();
	atomic_inc(&snapshot_device_available);
 Unlock:
	mutex_unlock(&pm_mutex);

	stop = cpu_clock(smp_processor_id());

	total_time = (stop - snapshot_restore_start);
	remaind = do_div(total_time, 1000000);

	resume_time = (start - snapshot_restore_start);
	remaind = do_div(resume_time , 1000000);

	remount_time = (stop_remount - start_remount);
	remaind = do_div(remount_time , 1000000);

	etc_time = total_time - resume_time;
	etc_time = etc_time - remount_time;
	do_div(profile_dpm_resume , 1000000);

    sprintf(message, "quickboot profile kernel - total [%lldms], resume [%lldms] remount [%lldms] etc [%lldms] dmp resume[%lldms]\n",
    	   total_time,
    	   resume_time,
    	   remount_time,
    	   etc_time,
    	   profile_dpm_resume
    	   );

   slogi(message);
   printk(message);

   do_hibernate_boot = 0;

   tcc_usb_notification();
   return error;
}


/**
 * software_resume - Resume from a saved hibernation image.
 *
 * This routine is called as a late initcall, when all devices have been
 * discovered and initialized already.
 *
 * The image reading code is called to see if there is a hibernation image
 * available for reading.  If that is the case, devices are quiesced and the
 */

void *read_file(const char *fn, unsigned *_sz)
{
    char *data;
    int sz;
    int fd;

    data = 0;
    fd = sys_open(fn, O_RDONLY, 0);
    if(fd < 0) return 0;

    sz = sys_lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(sys_lseek(fd, 0, SEEK_SET) != 0) goto oops;

    data = (char*) vmalloc(sz + 2);
    //data = (char*) vmalloc(sz + 2);
    if(data == 0) goto oops;

    if(sys_read(fd, data, sz) != sz) goto oops;
    sys_close(fd);
    data[sz] = '\n';
    data[sz+1] = 0;
    if(_sz) *_sz = sz;
    return data;

oops:
    sys_close(fd);
    if(data != 0) vfree(data);
    return 0;
}


static int insmod(const char *filename)
{
    void *module;
    void __user *umodule;
    void __user *uoptions;
    char options[2];
    unsigned size;
    int ret;

    options[0] = '\0';
    module = read_file(filename, &size);
    if (!module)
        return -1;

    umodule =  kmalloc(size + 2, GFP_USER);
    
    copy_to_user(umodule, module, size + 2);
    vfree(module);

    uoptions =  kmalloc(2, GFP_USER);
    copy_to_user(uoptions, options, 2);
	
    ret =sys_init_module(umodule, size, uoptions);

    kfree(umodule);
    kfree(uoptions);

    return ret;
}


static int load_mtd_module(void) {

	#if 0
	static char * path = "insmod";
	char *argv[] = { path,  "/lib/modules/tcc_mtd.ko", NULL};
	static char *envp[] = { "HOME=/",
				"TERM=linux",
				"PATH=/sbin:/usr/inssbin:/bin:/usr/bin",
				NULL };

	return call_usermodehelper(path,
				   argv, envp, UMH_WAIT_PROC);
	#else

	return insmod("/lib/modules/tcc_mtd.ko");
	#endif
}

static int load_nand_module(void) {
	return insmod("/lib/modules/tcc_nand.ko");
}


/*
 * contents of memory is restored from the saved image.
 *
 * If this is successful, control reappears in the restored target kernel in
 * hibernation_snaphot() which returns to hibernate().  Otherwise, the routine
 * attempts to recover gracefully and make the kernel return to the normal mode
 * of operation.
 */
static int software_resume(void)
{
	int error;
	unsigned int flags;

	/*
	 * If the user said "noresume".. bail out early.
	 */
	if (noresume)
		return 0;

	if( !strncmp(resume_file, "/dev/block/mtd", 14)  || !strncmp(resume_file, "/dev/block/ndd", 14)) {
           error = load_nand_module();
           pr_debug("load_nand_module result = %d\n", error);

           error = load_mtd_module();
           pr_debug("load_mtd_module result = %d\n", error);
	}

	/*
	 * name_to_dev_t() below takes a sysfs buffer mutex when sysfs
	 * is configured into the kernel. Since the regular hibernate
	 * trigger path is via sysfs which takes a buffer mutex before
	 * calling hibernate functions (which take pm_mutex) this can
	 * cause lockdep to complain about a possible ABBA deadlock
	 * which cannot happen since we're in the boot code here and
	 * sysfs can't be invoked yet. Therefore, we use a subclass
	 * here to avoid lockdep complaining.
	 */
	mutex_lock_nested(&pm_mutex, SINGLE_DEPTH_NESTING);

	if (swsusp_resume_device)
		goto Check_image;

	if (!strlen(resume_file)) {
		error = -ENOENT;
		goto Unlock;
	}

	pr_debug("PM: Checking hibernation image partition %s\n", resume_file);

	/* Check if the device is there */
	swsusp_resume_device = name_to_dev_t(resume_file);
	if (!swsusp_resume_device) {
		/*
		 * Some device discovery might still be in progress; we need
		 * to wait for this to finish.
		 */
		wait_for_device_probe();
		/*
		 * We can't depend on SCSI devices being available after loading
		 * one of their modules until scsi_complete_async_scans() is
		 * called and the resume device usually is a SCSI one.
		 */
		scsi_complete_async_scans();

		swsusp_resume_device = name_to_dev_t(resume_file);
		if (!swsusp_resume_device) {
			error = -ENODEV;
			goto Unlock;
		}
	}

 Check_image:
	pr_debug("PM: Hibernation image partition %d:%d present\n",
		MAJOR(swsusp_resume_device), MINOR(swsusp_resume_device));

#ifdef CONFIG_SNAPSHOT_BOOT
	goto Unlock;
#else
	pr_debug("PM: Looking for hibernation image.\n");
	error = swsusp_check();
	if (error)
		goto Unlock;
#endif

	/* The snapshot device should not be opened while we're running */
	if (!atomic_add_unless(&snapshot_device_available, -1, 0)) {
		error = -EBUSY;
		swsusp_close(FMODE_READ);
		goto Unlock;
	}

	pm_prepare_console();
	error = pm_notifier_call_chain(PM_RESTORE_PREPARE);
	if (error)
		goto close_finish;

	error = usermodehelper_disable();
	if (error)
		goto close_finish;

	error = create_basic_memory_bitmaps();
	if (error)
		goto close_finish;

	pr_debug("PM: Preparing processes for restore.\n");
	error = prepare_processes();
	if (error) {
		swsusp_close(FMODE_READ);
		goto Done;
	}

	pr_debug("PM: Loading hibernation image.\n");

	error = swsusp_read(&flags);
	swsusp_close(FMODE_READ);
	if (!error)
		hibernation_restore(flags & SF_PLATFORM_MODE);

	printk(KERN_ERR "PM: Failed to load hibernation image, recovering.\n");
	swsusp_free();
	thaw_processes();
 Done:
	free_basic_memory_bitmaps();
	usermodehelper_enable();
 Finish:
	pm_notifier_call_chain(PM_POST_RESTORE);
	pm_restore_console();
	atomic_inc(&snapshot_device_available);
	/* For success case, the suspend path will release the lock */
 Unlock:
	mutex_unlock(&pm_mutex);
	pr_debug("PM: Hibernation image not present or could not be loaded.\n");
	return error;
close_finish:
	swsusp_close(FMODE_READ);
	goto Finish;
}

late_initcall(software_resume);


static const char * const hibernation_modes[] = {
	[HIBERNATION_PLATFORM]	= "platform",
	[HIBERNATION_SHUTDOWN]	= "shutdown",
	[HIBERNATION_REBOOT]	= "reboot",
	[HIBERNATION_TEST]	= "test",
	[HIBERNATION_TESTPROC]	= "testproc",
};

/*
 * /sys/power/disk - Control hibernation mode.
 *
 * Hibernation can be handled in several ways.  There are a few different ways
 * to put the system into the sleep state: using the platform driver (e.g. ACPI
 * or other hibernation_ops), powering it off or rebooting it (for testing
 * mostly), or using one of the two available test modes.
 *
 * The sysfs file /sys/power/disk provides an interface for selecting the
 * hibernation mode to use.  Reading from this file causes the available modes
 * to be printed.  There are 5 modes that can be supported:
 *
 *	'platform'
 *	'shutdown'
 *	'reboot'
 *	'test'
 *	'testproc'
 *
 * If a platform hibernation driver is in use, 'platform' will be supported
 * and will be used by default.  Otherwise, 'shutdown' will be used by default.
 * The selected option (i.e. the one corresponding to the current value of
 * hibernation_mode) is enclosed by a square bracket.
 *
 * To select a given hibernation mode it is necessary to write the mode's
 * string representation (as returned by reading from /sys/power/disk) back
 * into /sys/power/disk.
 */

static ssize_t disk_show(struct kobject *kobj, struct kobj_attribute *attr,
			 char *buf)
{
	int i;
	char *start = buf;

	for (i = HIBERNATION_FIRST; i <= HIBERNATION_MAX; i++) {
		if (!hibernation_modes[i])
			continue;
		switch (i) {
		case HIBERNATION_SHUTDOWN:
		case HIBERNATION_REBOOT:
		case HIBERNATION_TEST:
		case HIBERNATION_TESTPROC:
			break;
		case HIBERNATION_PLATFORM:
			if (hibernation_ops)
				break;
			/* not a valid mode, continue with loop */
			continue;
		}
		if (i == hibernation_mode)
			buf += sprintf(buf, "[%s] ", hibernation_modes[i]);
		else
			buf += sprintf(buf, "%s ", hibernation_modes[i]);
	}
	buf += sprintf(buf, "\n");
	return buf-start;
}

static ssize_t disk_store(struct kobject *kobj, struct kobj_attribute *attr,
			  const char *buf, size_t n)
{
	int error = 0;
	int i;
	int len;
	char *p;
	int mode = HIBERNATION_INVALID;

	p = memchr(buf, '\n', n);
	len = p ? p - buf : n;

	mutex_lock(&pm_mutex);
	for (i = HIBERNATION_FIRST; i <= HIBERNATION_MAX; i++) {
		if (len == strlen(hibernation_modes[i])
		    && !strncmp(buf, hibernation_modes[i], len)) {
			mode = i;
			break;
		}
	}
	if (mode != HIBERNATION_INVALID) {
		switch (mode) {
		case HIBERNATION_SHUTDOWN:
		case HIBERNATION_REBOOT:
		case HIBERNATION_TEST:
		case HIBERNATION_TESTPROC:
			hibernation_mode = mode;
			break;
		case HIBERNATION_PLATFORM:
			if (hibernation_ops)
				hibernation_mode = mode;
			else
				error = -EINVAL;
		}
	} else
		error = -EINVAL;

	if (!error)
		pr_debug("PM: Hibernation mode set to '%s'\n",
			 hibernation_modes[mode]);
	mutex_unlock(&pm_mutex);
	return error ? error : n;
}

power_attr(disk);

static ssize_t resume_show(struct kobject *kobj, struct kobj_attribute *attr,
			   char *buf)
{
	return sprintf(buf,"%d:%d\n", MAJOR(swsusp_resume_device),
		       MINOR(swsusp_resume_device));
}

static ssize_t resume_store(struct kobject *kobj, struct kobj_attribute *attr,
			    const char *buf, size_t n)
{
	unsigned int maj, min;
	dev_t res;
	int ret = -EINVAL;

	if (sscanf(buf, "%u:%u", &maj, &min) != 2)
		goto out;

	res = MKDEV(maj,min);
	if (maj != MAJOR(res) || min != MINOR(res))
		goto out;

	mutex_lock(&pm_mutex);
	swsusp_resume_device = res;
	mutex_unlock(&pm_mutex);
	printk(KERN_INFO "PM: Starting manual resume from disk\n");
	noresume = 0;
	software_resume();
	ret = n;
 out:
	return ret;
}

power_attr(resume);

static ssize_t image_size_show(struct kobject *kobj, struct kobj_attribute *attr,
			       char *buf)
{
	return sprintf(buf, "%lu\n", image_size);
}

static ssize_t image_size_store(struct kobject *kobj, struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned long size;

	if (sscanf(buf, "%lu", &size) == 1) {
		image_size = size;
		return n;
	}

	return -EINVAL;
}

power_attr(image_size);

static ssize_t reserved_size_show(struct kobject *kobj,
				  struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", reserved_size);
}

static ssize_t reserved_size_store(struct kobject *kobj,
				   struct kobj_attribute *attr,
				   const char *buf, size_t n)
{
	unsigned long size;

	if (sscanf(buf, "%lu", &size) == 1) {
		reserved_size = size;
		return n;
	}

	return -EINVAL;
}

power_attr(reserved_size);

static struct attribute * g[] = {
	&disk_attr.attr,
	&resume_attr.attr,
	&image_size_attr.attr,
	&reserved_size_attr.attr,
	NULL,
};


static struct attribute_group attr_group = {
	.attrs = g,
};


static int __init pm_disk_init(void)
{
	return sysfs_create_group(power_kobj, &attr_group);
}

core_initcall(pm_disk_init);


static int __init resume_setup(char *str)
{
	if (noresume)
		return 1;

	strncpy( resume_file, str, 255 );
	return 1;
}

static int __init resume_offset_setup(char *str)
{
	unsigned long long offset;

	if (noresume)
		return 1;

	if (sscanf(str, "%llu", &offset) == 1)
		swsusp_resume_block = offset;

	return 1;
}

static int __init hibernate_setup(char *str)
{
	if (!strncmp(str, "noresume", 8))
		noresume = 1;
	else if (!strncmp(str, "nocompress", 10))
		nocompress = 1;
	return 1;
}

static int __init noresume_setup(char *str)
{
	noresume = 1;
	return 1;
}

__setup("noresume", noresume_setup);
__setup("resume_offset=", resume_offset_setup);
__setup("resume=", resume_setup);
__setup("hibernate=", hibernate_setup);
