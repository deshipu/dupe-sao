#include "ch32v20x.h"
#include "lcd.h"
#include "data.h"


int main(void) {
    SystemInit();
    Delay_Init();
    lcd_init();
    lcd_fill(0, 0, LCD_W, LCD_H, palette[0]);
    while (1) {
        lcd_blit16(22, 115 - 76, 84, 76, face1_data, palette);
        Delay_Ms(600);
        lcd_blit16(22, 115 - 76, 84, 76, face2_data, palette);
        Delay_Ms(600);
        lcd_blit16(22, 115 - 76, 84, 76, face3_data, palette);
        Delay_Ms(100);
        lcd_blit16(22, 115 - 76, 84, 76, face2_data, palette);
        Delay_Ms(600);
    }
}
