#include <list.h>

struct bus_type {
	const char		*name;

	int (*match)(struct device *dev, struct device_driver *drv);
	int (*probe)(struct device *dev);
	int (*remove)(struct device *dev);

	struct list_head list_devices;
	struct list_head list_drivers;

    int drivers_autoprobe;
};

struct device {
	const struct device_type *type;
	struct bus_type	*bus;		/* type of bus device is on */
	
	struct device_driver *driver;	/* which driver has allocated this device */

	struct list_head list_device;
};

struct device_driver {
	const char		*name;
	struct bus_type		*bus;

	int (*probe) (struct device *dev);
	int (*remove) (struct device *dev);

	struct list_head list_driver;
};

typedef unsigned int resource_size_t;

struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	unsigned long desc;
	struct resource *parent, *sibling, *child;
};

struct platform_device {
	const char	*name;
	struct device	dev;

	int		num_resources;
	struct resource	*resource;

	const struct platform_device_id	*id_entry;
};

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
};
