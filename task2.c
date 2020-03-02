#include <s3c2440_regs.h>
#include <lcd.h>

void fun_task2(void)
{
	volatile unsigned long i, j;

	while (1) {
		GPFDAT = ~(1 << 5);
		draw_rect(80, 80, 120, 120, 0x00ff00);

		for (i = 0; i < 800; ++i)
			for (j = 0; j < 800; ++j);

		GPFDAT = 0xff;
		draw_rect(80, 80, 120, 120, 0xffffff);

		for (i = 0; i < 800; ++i)
			for (j = 0; j < 800; ++j);

//		switch_to(&task_array[1]);
	}
}

