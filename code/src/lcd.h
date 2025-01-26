#ifndef __LCD_H__
#define __LCD_H__

#define LCD_W 128
#define LCD_H 115

void lcd_init();
void lcd_blit(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
	      const uint8_t data[]);
void lcd_fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
	      uint16_t color);
void lcd_blit16(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
	        const uint8_t data[], const uint16_t palette[]);

#endif /* __LCD_H__ */
