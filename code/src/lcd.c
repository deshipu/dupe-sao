#include "stdio.h"
#include "ch32v20x.h"
#include "lcd.h"

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5)	//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)	//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_0)	//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_0)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_2)	//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_2)

#define LCD_CS_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_3)	//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_3)

#define LCD_READY()    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)

void lcd_byte(uint8_t data) {
    LCD_CS_Clr();
    LCD_READY();
    SPI_I2S_SendData(SPI1, data);
    Delay_Us(1);
    LCD_CS_Set();
}

void lcd_word(uint16_t data) {
    lcd_byte(data >> 8);
    lcd_byte(data);
}

void lcd_register(uint8_t data) {
    LCD_DC_Clr();
    lcd_byte(data);
    LCD_DC_Set();
}

void lcd_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    lcd_register(0x2a);
    lcd_word(x1);
    lcd_word(x2);
    lcd_register(0x2b);
    lcd_word(y1 + 13);
    lcd_word(y2 + 13);
    lcd_register(0x2c);
}

void lcd_gpio_init() {
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    SPI_InitTypeDef SPI_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // DC (PA2) & CS (PA3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RST (PB0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SPI_CLK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SPI_MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SPI_MOSI
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
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

void lcd_fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
	      uint16_t color) {
    uint16_t size = width * height * 2;

    lcd_address(x, y, x + width - 1, y + height - 1);
    LCD_CS_Clr();
    for (uint16_t i = 0; i < size; i++) {
        LCD_READY();
	SPI_I2S_SendData(SPI1, color >> 8);
        LCD_READY();
	SPI_I2S_SendData(SPI1, color);
    }
    LCD_CS_Set();
}

void lcd_blit(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
	      const uint8_t data[]) {
    uint16_t size = width * height * 2;

    lcd_address(x, y, x + width - 1, y + height - 1);
    LCD_CS_Clr();
    for (uint16_t i = 0; i < size; i++) {
        LCD_READY();
	SPI_I2S_SendData(SPI1, data[i]);
    }
    LCD_CS_Set();
}

void lcd_blit16(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
	        const uint8_t data[], const uint16_t palette[]) {
    uint16_t size = (width * height) >> 1; // assume it's even
    uint16_t color;

    lcd_address(x, y, x + width - 1, y + height - 1);
    LCD_CS_Clr();
    for (uint16_t i = 0; i < size; i++) {
        color = palette[data[i] >> 4];
        LCD_READY();
	SPI_I2S_SendData(SPI1, color >> 8);
        LCD_READY();
	SPI_I2S_SendData(SPI1, color);
        color = palette[data[i] & 0x0f];
        LCD_READY();
	SPI_I2S_SendData(SPI1, color >> 8);
        LCD_READY();
	SPI_I2S_SendData(SPI1, color);
    }
    LCD_CS_Set();
}

void lcd_init() {
    lcd_gpio_init();

    LCD_RES_Clr();
    Delay_Ms(10);
    LCD_RES_Set();
    Delay_Ms(10);

    lcd_register(0x11);		// sleep out
    Delay_Ms(5);

    lcd_register(0x36);		// madctl
    lcd_byte(0xc8);
    lcd_register(0x3a);		// colmod
    lcd_byte(0x05);		// rgb565

    lcd_register(0x21);		// invert

    lcd_register(0x29);		// on
}
