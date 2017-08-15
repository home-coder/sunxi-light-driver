/*
 * lcd backlight control for test. 2017.4.17
 *
 */

#include <linux/kernel.h>                                                                                                                        
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <mach/sys_config.h>
#include <linux/gpio.h>
#include <linux/pinctrl/pinconf-sunxi.h>
#include <linux/pinctrl/consumer.h>

#include <linux/kthread.h>
#include <linux/earlysuspend.h>

#define lcd_backlight_msg(...)    do {printk("[lcd_backlight]: "__VA_ARGS__);} while(0)

static struct _lcd_backlight_info {
	int backlight_used;
	int backlight_on;
	struct task_struct *tsk;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
}lcd_backlight_info;

static char *lcd_backlight = "lcd_backlight";

static struct platform_device lcd_backlight_dev = {
	.name           = "lcd_backlight",
};

#ifdef CONFIG_PM
static int lcd_backlight_suspend(struct device *dev)
{
	printk("lcd_backlight_suspend---------------------\n");
	return 0;
}

static int lcd_backlight_resume(struct device *dev)
{
	return 0;
}

static struct dev_pm_ops lcd_backlight_dev_pm_ops = {
	.suspend    = lcd_backlight_suspend,
	.resume     = lcd_backlight_resume,
};
#endif

int sunxi_gpio_req(struct gpio_config *gpio)
{
	int            ret = 0;
	char           pin_name[8] = {0};
	unsigned long  config;

	sunxi_gpio_to_name(gpio->gpio, pin_name);
	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_FUNC, gpio->mul_sel);
	ret = pin_config_set(SUNXI_PINCTRL, pin_name, config);
	if (ret) {
		return -1;
	}

	if (gpio->pull != GPIO_PULL_DEFAULT){
		config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_PUD, gpio->pull);
		ret = pin_config_set(SUNXI_PINCTRL, pin_name, config);
		if (ret) {
			return -1;
		}
	}

	if (gpio->drv_level != GPIO_DRVLVL_DEFAULT){
		config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_DRV, gpio->drv_level);
		ret = pin_config_set(SUNXI_PINCTRL, pin_name, config);
		if (ret) {
			return -1;
		}
	}

	if (gpio->data != GPIO_DATA_DEFAULT) {
		config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_DAT, gpio->data);
		ret = pin_config_set(SUNXI_PINCTRL, pin_name, config);
		if (ret) {
			return -1;
		}
	}

	return 0;
}

static int lcd_backlight_get_res(void)
{
	//获取的gpio是数字 ?
	script_item_value_type_e type;
	script_item_u val; 
	struct gpio_config  *gpio_p = NULL;

	type = script_get_item(lcd_backlight, "backlight_used", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
		lcd_backlight_msg("------------failed to fetch lcd backlight configuration!\n");
		return -1;
	}
	if (!val.val) {
		lcd_backlight_msg("-----------no lcd backlight used in configuration\n");
		return -1;
	}
	lcd_backlight_info.backlight_used = val.val;

	lcd_backlight_info.backlight_on = -1;
	type = script_get_item(lcd_backlight, "backlight_on", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_PIO!=type)
		lcd_backlight_msg("-----------------get lcd backlight_on gpio failed\n");
	else {
		gpio_p = &val.gpio;
		lcd_backlight_info.backlight_on = gpio_p->gpio;
		sunxi_gpio_req(gpio_p);
	}

	return 0;
}

static int flick_func(void *noused)
{
	int i = 0;

	for (; i < 3; i++) {

		msleep(500);
		msleep(500);
		printk("--------------set 1------\n");
		gpio_direction_output(lcd_backlight_info.backlight_on, 1);
		gpio_set_value(lcd_backlight_info.backlight_on, 1);
		msleep(500);
		msleep(500);
		printk("--------------set 0------\n");
		gpio_set_value(lcd_backlight_info.backlight_on, 0);
	}
	gpio_free(lcd_backlight_info.backlight_on);

	return 0;
}

static void lcd_backlight_set_flick(void) {
	struct task_struct *ptsk = lcd_backlight_info.tsk;
	ptsk = kthread_run(flick_func, NULL, "lcd flick");
	if (IS_ERR(ptsk)) {
		printk(KERN_INFO "-----------create kthread failed!\n");
	}
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void lcd_early_suspend(struct early_suspend *h)
{
	struct _lcd_backlight_info *lcd;
	lcd = container_of(h, struct _lcd_backlight_info, early_suspend);
	printk("------early suspend-----\n");
//do nothing, put it to Runtime
	
}
#endif

static int __devinit lcd_backlight_probe(struct platform_device *pdev) 
{
	//0.配置fex方式:       方便查看类似板级文件的使用情况，需要驱动中对此fex配置进行解析。
	//  另外就是直接方式： 可以参考本目录下的test文件（AllWinner提供）是(bank - 1) *32 + offset来做：标准linux kernel形式
	lcd_backlight_msg("--------------lcd_backlight dev probe\n");
	//1.解析平台fex配置,获取资源
	lcd_backlight_get_res();	

	//2.创建线程闪屏30次，每次500ms
	lcd_backlight_set_flick();
	
	//0.注册early suspend
#ifdef CONFIG_HAS_EARLYSUSPEND
	lcd_backlight_info.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	lcd_backlight_info.early_suspend.suspend = lcd_early_suspend;
	printk("---------register early suspend---\n");
	register_early_suspend(&lcd_backlight_info.early_suspend);
#endif
	return 0;
}

static int __devexit lcd_backlight_remove(struct platform_device *pdev)
{
	lcd_backlight_msg("lcd_backlight dev remove\n");

	return 0;
} 

static struct platform_driver lcd_backlight_driver = {
	.driver = {
		.name  = "lcd_backlight",
		.owner = THIS_MODULE,
#ifdef CONFIG_PM
		.pm    = &lcd_backlight_dev_pm_ops,
#endif
	},
	.probe          = lcd_backlight_probe,
	.remove         = __devexit_p(lcd_backlight_remove),
};

static int __init lcd_backlight_init(void)
{
	printk("----------init-----------\n");
	platform_device_register(&lcd_backlight_dev);
	return platform_driver_register(&lcd_backlight_driver);
}

static void __exit lcd_backlight_exit(void)
{
	struct task_struct *ptsk = lcd_backlight_info.tsk;
	printk("----------exit-----------\n");
#if 0
	if (!IS_ERR(ptsk)){  
		int ret = kthread_stop(ptsk);  
		printk(KERN_INFO "------thread function has stop %ds\n", ret);  
	}
#endif
	platform_driver_unregister(&lcd_backlight_driver);
	platform_device_unregister(&lcd_backlight_dev);
}

module_init(lcd_backlight_init);
module_exit(lcd_backlight_exit);
MODULE_AUTHOR("can jiang");
MODULE_LICENSE("GPL"); 
