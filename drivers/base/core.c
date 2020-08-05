#include <core.h>


static inline int driver_match_device(struct device_driver *drv,
				      struct device *dev)
{
	printk("enter %s()\r\n", __func__);
	return drv->bus->match ? drv->bus->match(dev, drv) : 1;
}

int driver_probe_device(struct device_driver *drv, struct device *dev)
{
	int ret = 0;

	// printk("bus: '%s': %s: matched device %s with driver %s\n",
	// 	 drv->bus->name, __func__, dev_name(dev), drv->name);

	// printk("bus: '%s': %s: probing driver %s with device %s\n",
	// 	 drv->bus->name, __func__, drv->name, dev_name(dev));
	printk("enter %s()\r\n", __func__);

	dev->driver = drv;

	if (dev->bus->probe) {
		ret = dev->bus->probe(dev);
		if (ret)
			return ret;
	} else if (drv->probe) {
		ret = drv->probe(dev);
		if (ret)
			return ret;
	}

	return ret;
}

int driver_register(struct device_driver *drv)
{
	int ret;
	struct device *dev;

	printk("enter %s()\r\n", __func__);

	list_add_tail(&drv->list_driver, &drv->bus->list_drivers);

	list_for_each_entry(dev, &drv->bus->list_devices, list_device) {
		ret = driver_match_device(drv, dev);
		if (ret == 0) {
			/* no match */
			return 0;
		} else if (ret < 0) {
			printk("Bus failed to match device: %d", ret);
			return ret;
		} /* ret > 0 means positive match */

		driver_probe_device(drv, dev);
	}

	return 0;
}

int device_register(struct device *dev)
{
	printk("enter %s()\r\n", __func__);
	list_add_tail(&dev->list_device, &dev->bus->list_devices);
}