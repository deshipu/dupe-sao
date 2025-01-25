#include "stdio.h"
#include "ch32v20x.h"
#include "lcd.h"


void LCD_Writ_Bus(u8 dat) {
#if defined(SPI_GPIO)
    u8 i;

    LCD_CS_Clr();
    for (i = 0; i < 8; i++) {
	LCD_SCLK_Clr();
	if (dat & 0x80) {
	    LCD_MOSI_Set();
	} else {
	    LCD_MOSI_Clr();
	}
	LCD_SCLK_Set();
	dat <<= 1;
    }
    LCD_CS_Set();
#elif defined(SPI_HW)
    LCD_CS_Clr();
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, dat);
    Delay_Us(1);
    LCD_CS_Set();
#endif
}

void LCD_WR_DATA8(u8 dat) {
    LCD_Writ_Bus(dat);
}

void LCD_WR_DATA(u16 dat) {
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
}

void LCD_WR_REG(u8 dat) {
    LCD_DC_Clr();
    LCD_Writ_Bus(dat);
    LCD_DC_Set();
}

void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2) {
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA(y1 + 13);
	LCD_WR_DATA(y2 + 13);
	LCD_WR_REG(0x2c);
}


int lcd_gpio_init() {
#if defined(SPI_GPIO)
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin =
	GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif defined(SPI_HW)
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    SPI_InitTypeDef SPI_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /* DC (PB10) & RESET (PB11) & CS (PB12) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* RST (PB0) */
    //GPIO_SetBits(GPIOB, GPIO_Pin_0); // RST (PB0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* SPI_CLK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI_MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI_MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
#endif

    return 0;
}

void LCD_Fill_Slow(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color) {
    u16 i, j;

    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);
    for (i = ysta; i < yend; i++) {
	for (j = xsta; j < xend; j++) {
	    LCD_WR_DATA(color);
	}
    }
}

void LCD_Fill_Fast(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color) {
    u16 i, j;

    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);

    LCD_CS_Clr();

    for (i = ysta; i < yend; i++) {
	for (j = xsta; j < xend; j++) {
	    //LCD_WR_DATA(color);

	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI1, color >> 8);
	    //Delay_Us(1);
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI1, color);
	}
    }

    LCD_CS_Set();

}

void LCD_ShowPicture_Slow(u16 x, u16 y, u16 length, u16 width,
			  const u8 pic[]) {
    u16 i, j;
    u32 k = 0;

    LCD_Address_Set(x, y, x + length - 1, y + width - 1);
    for (i = 0; i < length; i++) {
	for (j = 0; j < width; j++) {
	    LCD_WR_DATA8(pic[k * 2]);
	    LCD_WR_DATA8(pic[k * 2 + 1]);
	    k++;
	}
    }
}

void LCD_ShowPicture_Fast(u16 x, u16 y, u16 length, u16 width,
			  const u8 pic[]) {
    u16 i, j;
    u32 k = 0;

    LCD_Address_Set(x, y, x + length - 1, y + width - 1);

    LCD_CS_Clr();

    for (i = 0; i < length; i++) {
	for (j = 0; j < width; j++) {
	    //LCD_WR_DATA8(pic[k*2]);
	    //LCD_WR_DATA8(pic[k*2+1]);

	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI1, pic[k * 2]);
	    //Delay_Us(1);
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI1, pic[k * 2 + 1]);
	    k++;
	}
    }

    LCD_CS_Set();
}

int lcd_init() {
    lcd_gpio_init();

    LCD_RES_Clr();
    Delay_Ms(100);

    LCD_RES_Set();
    Delay_Ms(100);

//    LCD_BLK_Set();
    Delay_Ms(100);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11);		//Sleep out
    Delay_Ms(120);		//Delay 120ms
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x36);
    LCD_WR_DATA8(0xC8);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);
    LCD_WR_REG(0x21);
    LCD_WR_REG(0x29);

    return 0;
}
