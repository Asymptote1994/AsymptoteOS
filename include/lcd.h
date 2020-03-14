#ifndef __LCD_H__
#define __LCD_H__

void draw_pixel(unsigned int x, unsigned int y, unsigned int color);
void clear_screen(unsigned int color);
void lcd_init();
void draw_rect(unsigned int x_start, unsigned int y_start, 
				unsigned int x_end, unsigned int y_end, unsigned int color);

#endif

