/*
 * listen to power key event, 2017-4.-27
 *
 */

#include <linux/io.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinconf-sunxi.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/list.h>
#include <mach/sys_config.h>
#include <mach/platform.h>
#include <mach/gpio.h>

static int power_key_listen_probe(struct platform_device *pdev)
{

}

static struct platform_driver power_key_listen_driver = {
	.probe		= power_key_listen_probe,
	.driver		= {
		.name		= "power_key",
		.owner		= THIS_MODULE,
	},
};

static struct platform_device power_key_listen_device = {
	.name		= "power_key",
	.id			= PLATFORM_DEVID_NONE,
};

static int __init power_key_listen_init(void)
{
	platform_device_register();
	platform_driver_register();

	return 0;
}

static void __exit power_key_listen_exit(void)
{
	platform_device_unregister();
	platform_driver_unregister();
}

module_init(power_key_listen_init);
module_exit(power_key_listen_exit);

MODULE_AUTHOR();
MODULE_LICENSE();
