#include <mbed.h>
#include "TS_DISCO_F429ZI.h"
#include "LCD_DISCO_F429ZI.h"

#include <my_driver.h>

DigitalOut led(LED2);
SPI spi(PF_9, PF_8, PF_7);
DigitalOut cs(PC_1);

InterruptIn button(USER_BUTTON);
int Is_enable=0;
void Gyro_On() {
    led != led;
}

LCD_DISCO_F429ZI lcd;
TS_DISCO_F429ZI ts;

int main() {
    uint8_t text[30];
    BSP_LCD_SetFont(&Font24);

    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"SEX DEMO", CENTER_MODE);

    L3GD20Gyroscope gyroscope(&spi, PC_1);
    int err = gyroscope.init();

    int16_t w_dps[3];
    int count = 0;
    
    button.fall(&Gyro_On);

    wait_us(1000000);

    BSP_LCD_SetFont(&Font16);
    lcd.Clear(LCD_COLOR_ORANGE);
    lcd.SetBackColor(LCD_COLOR_DARKRED);
    lcd.SetTextColor(LCD_COLOR_WHITE);

    while(1) {
        wait_us(1000000);
        gyroscope.read_data_16(w_dps);

        int x = float(w_dps[0]) * 0.00875f;
        int y = float(w_dps[1]) * 0.00875f;
        int z = float(w_dps[2]) * 0.00875f;
        
        // printf("%04d | wx: %d , wy: %d, wz: %d\n", count, x, y, z);

        //led = !led;
        count++;

        sprintf((char*)text, "wx=%d wy=%d wz=%d", x, y, z);
        lcd.DisplayStringAt(0, LINE(7), (uint8_t *)&text, CENTER_MODE);        
    }
}

