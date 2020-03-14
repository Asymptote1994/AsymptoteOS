#include <s3c2440_regs.h>
#include <lcd.h>

void fun_task3(void)
{
	volatile unsigned long i, j;

	while (1) {
		GPFDAT = ~(1 << 6);
		draw_rect(150, 150, 190, 190, 0xff0000);

		for (i = 0; i < 800; ++i)
			for (j = 0; j < 800; ++j);

		GPFDAT = 0xff;
		draw_rect(150, 150, 190, 190, 0xffffff);

		for (i = 0; i < 800; ++i)
			for (j = 0; j < 800; ++j);

//		switch_to(&task_array[0]);
	}
}

