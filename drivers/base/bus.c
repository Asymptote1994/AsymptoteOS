#include <core.h>

int bus_register(struct bus_type *bus)
{
	list_init(&(bus->list_devices));
	list_init(&(bus->list_drivers));

	return 0;
}