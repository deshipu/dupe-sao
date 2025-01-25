#include "ch32v20x.h"
#include "stdio.h"
#include "lcd.h"
#include "dupe.h"


int main(void) {
    u8 i = 0;
    //SystemInit();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    Delay_Ms(100);
    SystemCoreClockUpdate();

    lcd_init();

    LCD_Fill_Fast(0, 0, LCD_W, LCD_H, 0x2897);

    //LCD_ShowPicture_Slow(29,48,182,144,gImage_test);
    LCD_ShowPicture_Fast(24, 115 - 74, 80, 74, image_data);

#if 1
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    while (1) {
	GPIO_WriteBit(GPIOA, GPIO_Pin_1,
		      (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
	Delay_Ms(100);
    }
#endif

}
