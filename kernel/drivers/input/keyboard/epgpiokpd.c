#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>

#include <linux/platform_device.h>
#include <linux/input.h>
#include <asm/gpio.h>
#include <linux/gpio.h>
#include <mach/hardware.h>
#include <mach/bsp.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define DEV_NAME "ep-button"
#define MAX_BUTTON_CNT (2)

static struct timer_list timer;
static struct input_dev * input;

//static unsigned ep_key_gpio_list[1] = {TCC_GPE(27)};
static unsigned ep_key_gpio_list[1] = {TCC_GPG(14)};

static int ep_Keycode[MAX_BUTTON_CNT] = {KEY_POWER, KEY_MENU};

static int ep_button_history[MAX_BUTTON_CNT] = {0};
//extern int shang_gong_config_1;
//extern int shang_gong_config_2;
//#define PMU_CONFIG
#if defined(PMU_CONFIG) /* added macro PMU_CONFIG */
static volatile PPMU pPMU;
#endif /* PMU_CONFIG */

static void ep_button_timer_handler(unsigned long data)
{
	int flag;
	int i=0;
	printk("[## %s ##] %s in %d %d\n", DEV_NAME, __func__, (gpio_get_value(TCC_GPG(14))), (!gpio_get_value(TCC_GPG(14))));

	//for(i = 0; i < MAX_BUTTON_CNT; i++)
	{
		flag = gpio_get_value((ep_key_gpio_list[i]));

		printk("[## %s ##] %s flag %d, last %d\n", DEV_NAME, __func__, flag, ep_button_history[i]);
		if(flag != ep_button_history[i])
		{
			if(flag)
			{
			input_report_key(input, KEY_POWER, 1);
			input_event(input, EV_SYN, 0, 0);
			input_report_key(input, KEY_POWER, 0);
			input_event(input, EV_SYN, 0, 0);
			}
			else
			{
				input_report_key(input, KEY_POWER, 1);
				input_event(input, EV_SYN, 0, 0);
				input_report_key(input, KEY_POWER, 0);
				input_event(input, EV_SYN, 0, 0);
				// here cannot unlock
				// simulate the menu key to unlock screen
//					input_report_key(input, KEY_MENU, 1);
//					input_event(input, EV_SYN, 0, 0);
//					input_report_key(input, KEY_MENU, 0);
//					input_event(input, EV_SYN, 0, 0);
			}
			ep_button_history[i] = flag;
		}
	}

	/* Kernel Timer restart */
	mod_timer(&timer,jiffies + msecs_to_jiffies(1000));//HZ/100);
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ep_button_early_suspend(struct early_suspend *h)
{
	printk("[##eeeeeeeee %s ##] %s in\n", DEV_NAME, __func__);
}

static void ep_button_late_resume(struct early_suspend *h)
{
	printk("[##eeeeeeeee %s ##] %s in\n", DEV_NAME, __func__);
	// simulate the menu key to unlock screen
	input_report_key(input, KEY_MENU, 1);
	input_event(input, EV_SYN, 0, 0);
	input_report_key(input, KEY_MENU, 0);
	input_event(input, EV_SYN, 0, 0);
}

#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
struct early_suspend mydev_early_suspend = {
	.suspend	= ep_button_early_suspend,
	.resume	= ep_button_late_resume,
//	.level 	= EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1,
	.level	= EARLY_SUSPEND_LEVEL_DISABLE_FB -2,
};
#endif

#ifdef CONFIG_PM
static int ep_button_suspend(struct platform_device *pdev, pm_message_t state)
{
	printk("[##eeeeeeeee %s ##] %s in\n", DEV_NAME, __func__);
	ep_button_history[0] = 1;
	return 0;
}

static int ep_button_resume(struct platform_device *pdev)
{
	printk("[##eeeeeeeee %s ##] %s in\n", DEV_NAME, __func__);
//	ep_button_history[0] = 0;
	return 0;
}
#else

#define ep_button_suspend	NULL
#define ep_button_resume	NULL

#endif

static int ep_button_probe(struct platform_device *pdev)
{
	int i;
	printk("[## %s ##] %s in\n", DEV_NAME, __func__);

	for(i = 0; i < MAX_BUTTON_CNT; i++) { 
		tcc_gpio_config((ep_key_gpio_list[i]), GPIO_FN(0) | GPIO_INPUT);
	//gpio_direction_input((ep_key_gpio_list[i]));
	}
	input = input_allocate_device();
	if(!input)
		return -ENOMEM;
	set_bit(EV_KEY, input->evbit);
	//set_bit(EV_REP, input->evbit); /* Repeat Key */

	for(i = 0; i < MAX_BUTTON_CNT; i++)
		set_bit(ep_Keycode[i], input->keybit);

	input->name = "ep-button";
	input->phys = "ep-button/input0";

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	input->keycode = ep_Keycode;
	if(input_register_device(input) != 0)
	{
		printk("ep-button input register device fail!!\n");
		input_free_device(input);
		return -ENODEV;
	}
#ifdef CONFIG_HAS_EARLYSUSPEND
	register_early_suspend(&mydev_early_suspend);
#endif
	/* Scan timer init */
	init_timer(&timer);

	timer.function = ep_button_timer_handler;

	timer.expires = jiffies + (HZ/100);
	add_timer(&timer);

#if defined(PMU_CONFIG) /* added macro PMU_CONFIG */
	shang_gong_config_2=1;
	pPMU = (PMU *)io_p2v(HwPMU_BASE);

//		while(pPMU->PWRDN_XIN.nREG&(1<<8)){
//			BITCLR(pPMU->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode =0
//		}
//		while(pPMU->PWRDN_XIN.nREG&(1<<4)){
//			BITCLR(pPMU->PWRDN_XIN.nREG, 1<<4); //IO_RTO : I/O retention mode =0
//		}
	//set wake-up trigger mode : edge
	pPMU->PMU_WKMOD0.nREG = 0xFFFFFFFF;
	pPMU->PMU_WKMOD1.nREG = 0xFFFFFFFF;
	//set wake-up polarity : default : active high
	pPMU->PMU_WKPOL0.nREG = 0x00000000;
	pPMU->PMU_WKPOL1.nREG = 0x00000000;

	pPMU->PMU_WKPOL0.bREG.GPIO_G14 = 1; //uart-rx(BT HOST WAKE UP) - Active Low-			   
	pPMU->PMU_WKUP0.bREG.GPIO_G14 = 1; //uart-rx(BT HOST WAKE UP)	
	pPMU->PMU_WKPOL0.bREG.RTC_WAKEUP = 0; //RTC_PMWKUP - Active High
	//set wake-up source
	pPMU->PMU_WKUP0.bREG.RTC_WAKEUP = 1; //RTC_PMWKUP - PMU WakeUp Enable
//		while(!(pPMU->PWRDN_XIN.nREG&(1<<8))){
//			BITSET(pPMU->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
//		}
//		while(!(pPMU->PWRDN_XIN.nREG&(1<<4))){
//			BITSET(pPMU->PWRDN_XIN.nREG, 1<<4); //IO_RTO : I/O retention mode disable=1
//		}
	
	printk("[##eeeee %s ##] %s config pmuwk\n", DEV_NAME, __func__);
#endif /* PMU_CONFIG */
	printk("ep button Initialized!!\n");
	return 0;
}

static int ep_button_remove(struct platform_device *pdev)
{
	printk("[## %s ##] %s in\n", DEV_NAME, __func__);
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&mydev_early_suspend);
#endif
	input_unregister_device(input);
	input_free_device(input);
	del_timer(&timer);
	return	0;
}

static struct platform_driver ep_button_device_driver = {
	.probe = ep_button_probe,
	.remove = ep_button_remove,
	.suspend = ep_button_suspend,
	.resume = ep_button_resume,
	.driver = {
		.name =	"ep-button",
		.owner = THIS_MODULE,
	}
};

static struct platform_device ep_device_button = {
	.name =	"ep-button",
	.id	= -1,
};

static int __init ep_button_init(void)
{
	printk("[## %s ##] %s in\n", DEV_NAME, __func__);
	platform_device_register(&ep_device_button);
	return platform_driver_register(&ep_button_device_driver);
}

static void __exit ep_button_exit(void)
{
	printk("[## %s ##] %s in\n", DEV_NAME, __func__);
	platform_driver_unregister(&ep_button_device_driver);
	platform_device_unregister(&ep_device_button);
}

module_init(ep_button_init);
module_exit(ep_button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shang.zhenhua <shang.zhenhua@excelpoint.com.cn>");
MODULE_DESCRIPTION("Keyboard driver for ep button.");
MODULE_ALIAS("platform:ep-button");

