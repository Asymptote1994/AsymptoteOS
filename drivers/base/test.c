#include <core.h>

static int s3c2440_mmc_probe(struct platform_device *pdev)
{
	printk("enter %s()\r\n", __func__);
	printk("device name is %s\r\n", pdev->name);

	return 0;
}

static struct platform_driver s3c2440_plat_mmc_driver = {
	.probe		= s3c2440_mmc_probe,
	.driver		= {
		.name	= "samsung,s3c2440-mmc",
	},
};

static struct platform_device s3c2440_plat_mmc_device = {
	.name	= "samsung,s3c2440-mmc",
};

int s3c2440_mmc_init(void)
{
	printk("enter %s()\r\n", __func__);
	platform_device_register(&s3c2440_plat_mmc_device);
	return platform_driver_register(&s3c2440_plat_mmc_driver);
}
