// #include <mbed.h>
// #include <GYRO_SENSOR.h>
// #include "TS_DISCO_F429ZI.h"
// #include "LCD_DISCO_F429ZI.h"

// #define radius 0.66
// #define ZERO_RATE_NOISE 0.280000

// DigitalOut led(LED2);
// SPI spi(PF_9, PF_8, PF_7);
// DigitalOut cs(PC_1);

// InterruptIn button(USER_BUTTON);
// int Is_enable=0;
// void Gyro_On() {
//     led != led;
// }

// LCD_DISCO_F429ZI lcd;
// TS_DISCO_F429ZI ts;

// float speed = 0;
// float dist=0;
// float x;
// float y;
// float z;
// float x1;
// float avg_speed = 0;
// uint8_t text1[30];
// uint8_t text2[30];
// uint8_t text3[30];

// void print() {
//   led != led;
//   static int cnt = 1;
//   speed = avg_speed * radius;
//   dist += speed;

//   sprintf((char*)text1, "Time Elapsed=%d sec", cnt);
//   lcd.DisplayStringAt(0, LINE(4), (uint8_t *)&text1, CENTER_MODE);
//   sprintf((char*)text2, "Speed=%0.2f km/hr", speed * 3.6 );
//   lcd.DisplayStringAt(0, LINE(6), (uint8_t *)&text2, CENTER_MODE);
//   sprintf((char*)text3, "Distance=%0.2f meters", dist);
//   lcd.DisplayStringAt(0, LINE(8), (uint8_t *)&text3, CENTER_MODE); 
//   cnt++;
// }

// int main() {
//     Ticker tick;

//     BSP_LCD_SetFont(&Font24);

//     lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"RTES:Project", CENTER_MODE);
//     lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"ECE-6483", CENTER_MODE);

//     Gyroscope gyroscope(&spi, PC_1);
//     int err = gyroscope.init();

//     int16_t w_dps[3];
//     int count = 0;
    
//     button.fall(&Gyro_On);

//     wait_us(1000000);

//     BSP_LCD_SetFont(&Font16);
//     lcd.Clear(LCD_COLOR_ORANGE);
//     lcd.SetBackColor(LCD_COLOR_DARKRED);
//     lcd.SetTextColor(LCD_COLOR_WHITE);

//     tick.attach(&print,1);
    
//     while(1) {

//         wait_us(100000);
//         gyroscope.read_data_16(w_dps);
        
//         x = w_dps[0] * 0.00875f;
//         y = w_dps[1] * 0.00875f;
//         z = w_dps[2] * 0.00875f;

//         x = x + ZERO_RATE_NOISE;
//         if(x < 0.30)
//             x = 0;

//         x1 = x * 0.0174533f;
        
//         avg_speed = gyroscope.average_Velocity(x1); //Circular queue to calculate average velocity
//         //float x1 = x * 0.0174533f;s
        
//         //printf("%04d | wx1: %f, wx: %f , temp: %d\n", count, x1,x, temp);
//         printf("wx:%f  wx1:%f  temp:%f\n",x,x1,avg_speed);
//         //printf("%04d | wx: %+7.2f dps, wy: %+7.2f dps, wz: %+7.2f dps\n", count, x, y, z);
//         //led = !led;
//         count++;
//     }
// }

