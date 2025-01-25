#ifndef __LCD_H__
#define __LCD_H__

//#define SPI_GPIO
#define SPI_HW
#define USE_HORIZONTAL 0


#define LCD_W 128
#define LCD_H 115

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0XF81F
#define GRED 	    0XFFE0
#define GBLUE	    0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 	    0XBC40
#define BRRED 	    0XFC07
#define GRAY  	    0X8430
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define LIGHTGREEN  0X841F
#define LGRAY 	    0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_0)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_0)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_2)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_2)

#define LCD_CS_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_3)//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_3)

//#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_9)//BLK
//#define LCD_BLK_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_9)

extern int lcd_init();
extern void LCD_ShowPicture_Fast(u16 x, u16 y, u16 length, u16 width,
				 const u8 pic[]);
extern void LCD_Fill_Fast(u16 xsta, u16 ysta, u16 xend, u16 yend,
			  u16 color);

#endif /* __LCD_H__ */
