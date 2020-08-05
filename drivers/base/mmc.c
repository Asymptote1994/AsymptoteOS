#include <core.h>

struct mmc_device {
	const char	*name;
	struct device	dev;

	int		num_resources;
	struct resource	*resource;

	const struct mmc_device_id	*id_entry;
	char *driver_override; /* Driver name to force a match */
};

struct mmc_driver {
	int (*probe)(struct mmc_device *);
	int (*remove)(struct mmc_device *);
	struct device_driver driver;
	const struct mmc_device_id *id_table;
};

#define to_mmc_driver(drv)	(container_of((drv), struct mmc_driver, driver))
#define to_mmc_device(x) container_of((x), struct mmc_device, dev)


static int mmc_match(struct device *dev, struct device_driver *drv)
{
	struct mmc_device *pdev = to_mmc_device(dev);

	/* fall-back to driver name match */
	return (strcmp(pdev->name, drv->name) == 0);
}

struct bus_type mmc_bus_type = {
	.name		= "mmc",
	.match		= mmc_match,
};


static int mmc_drv_probe(struct device *_dev)
{
	struct mmc_driver *drv = to_mmc_driver(_dev->driver);
	struct mmc_device *dev = to_mmc_device(_dev);
	int ret;

	if (drv->probe) {
		ret = drv->probe(dev);
		if (ret)
			return ret;
	}

	return ret;
}

int mmc_driver_register(struct mmc_driver *drv)
{
	drv->driver.bus = &mmc_bus_type;
	drv->driver.probe = mmc_drv_probe;

	return driver_register(&drv->driver);
}

int mmc_device_register(struct mmc_device *pdev)
{
	int ret;

	pdev->dev.bus = &mmc_bus_type;

	printk("Registering platform device '%s'\n", pdev->name);

	return device_register(&pdev->dev);
}

int mmc_bus_init(void)
{
	int error;

	error =  bus_register(&mmc_bus_type);

	return error;
}
