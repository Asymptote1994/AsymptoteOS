#include <core.h>

#define to_platform_driver(drv)	(container_of((drv), struct platform_driver, driver))
#define to_platform_device(x) container_of((x), struct platform_device, dev)


static int platform_match(struct device *dev, struct device_driver *drv)
{
	struct platform_device *pdev = to_platform_device(dev);

	printk("enter %s()\r\n", __func__);
	return (strcmp(pdev->name, drv->name) == 0);
}

struct bus_type platform_bus_type = {
	.name		= "platform",
	.match		= platform_match,
};


static int platform_drv_probe(struct device *_dev)
{
	struct platform_driver *drv = to_platform_driver(_dev->driver);
	struct platform_device *dev = to_platform_device(_dev);
	int ret;

	printk("enter %s()\r\n", __func__);

	if (drv->probe) {
		ret = drv->probe(dev);
		if (ret)
			return ret;
	}

	return ret;
}

int platform_driver_register(struct platform_driver *drv)
{
	drv->driver.bus = &platform_bus_type;
	drv->driver.probe = platform_drv_probe;

	printk("Registering platform driver '%s'\r\n", drv->driver.name);

	return driver_register(&drv->driver);
}

int platform_device_register(struct platform_device *pdev)
{
	int ret;

	pdev->dev.bus = &platform_bus_type;

	printk("Registering platform device '%s'\r\n", pdev->name);

	return device_register(&pdev->dev);
}

int platform_bus_init(void)
{
	int error;

	printk("enter %s()\r\n", __func__);

	error =  bus_register(&platform_bus_type);

	return error;
}
