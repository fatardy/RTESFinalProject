#include <mbed.h>
#include <GYRO_SENSOR.h>
#include "LCD_DISCO_F429ZI.h"
#include "TS_DISCO_F429ZI.h"

#define radius 0.66
#define ZERO_RATE_NOISE 0.280000

DigitalOut led(LED2);
SPI spi(PF_9, PF_8, PF_7);
DigitalOut cs(PC_1);

InterruptIn button(USER_BUTTON);
int Is_enable=0;

void Gyro_On() {
    led != led;
}

float speed = 0;
float dist = 0;
float gyroX;
float gyroY;
float gyroZ;
float gyroX1;
float avg_speed = 0;
uint8_t text1[30];
uint8_t text2[30];
uint8_t text3[30];

LCD_DISCO_F429ZI lcd;
TS_DISCO_F429ZI ts;

uint32_t screenWidth;
uint32_t screenHeight;
// width is 240; height is 320;
uint32_t screenMargin = 16; // defined for ui

TS_StateTypeDef TS_State;
uint16_t touchX, touchY;
// NOTE:(0, 0) is at the bottom left; (x, y) also register the black part of the screen;

int userHeight = 178;

// screens
enum Screens {
    SPLASH_SCREEN = 1,
    CONFIG_SCREEN,
    MAIN_SCREEN,
    RESULT_SCREEN,
};
int activeScreen = SPLASH_SCREEN;


void wait() {
    wait_us(1000000);
}

void splashScreenView() {
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    
    wait();

    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(0, LINE(3), (uint8_t *)"RTES", CENTER_MODE);
    lcd.SetTextColor(LCD_COLOR_MAGENTA);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Need For Speed", CENTER_MODE);

    wait();

    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.DrawLine(screenWidth / 4, 134, (screenWidth / 4) * 3, 134);
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_BLUE);
    lcd.DisplayStringAt(0, LINE(9), (uint8_t *)"Rituraj Singh", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"Aradhya Alamuru", CENTER_MODE);

    wait();

    // lcd.SetTextColor(LCD_COLOR_BLACK);
    // lcd.SetFont(&Font16);
    // lcd.DisplayStringAt(0, screenHeight - 32, (uint8_t *)"Tap to start", CENTER_MODE);
    
    lcd.SetTextColor(LCD_COLOR_LIGHTBLUE);
    lcd.SetBackColor(LCD_COLOR_LIGHTBLUE);
    lcd.FillRect(screenMargin, screenHeight - 52, screenWidth - (2 * screenMargin), 44);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, screenHeight - 36, (uint8_t *)"START", CENTER_MODE);

    while(1) {
        // lcd.DisplayStringAt(0, screenHeight - 32, (uint8_t *)"Tap to start", CENTER_MODE);
    
        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        printf("x - %d, y - %d\n", touchX, touchY);

        if ((TS_State.TouchDetected) && (touchY < 40)) {
            activeScreen = CONFIG_SCREEN;
            break;
        }

        // TODO: use interupts? currently blocking the while loop;
        // wait();
        // lcd.DisplayStringAt(0, screenHeight - 32, (uint8_t *)"            ", CENTER_MODE);
        // wait();
    }
}

void increaseHeight() {
    userHeight++;
}
void decreaseHeight() {
    userHeight--;
}

void configScreenView() {
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    
    wait();

    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(0, 40, (uint8_t *)"Select Height", CENTER_MODE);
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_GRAY);
    lcd.DisplayStringAt(0, 60, (uint8_t *)"(centimeters)", CENTER_MODE);
    
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"We use your height to", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(8), (uint8_t *)"calculate speed", CENTER_MODE);

    lcd.SetTextColor(LCD_COLOR_DARKMAGENTA);
    lcd.SetBackColor(LCD_COLOR_DARKMAGENTA);
    lcd.FillRect(screenMargin, screenHeight - 52, screenWidth - (2 * screenMargin), 44);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, screenHeight - 36, (uint8_t *)"GO!", CENTER_MODE);
    
    lcd.SetTextColor(LCD_COLOR_DARKRED);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font24);
        
    while(1) {
        sprintf((char*)text1, "<  %d  >", userHeight);
        lcd.DisplayStringAt(0, 170, (uint8_t *)&text1, CENTER_MODE);

        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        printf("x - %d, y - %d\n", touchX, touchY);

        if (TS_State.TouchDetected) {
            if (touchY < 40) {
                activeScreen = MAIN_SCREEN;
                break;
            }
            if ((touchY > 80) && (touchY < 160)) {
                if (touchX > 160) {
                    increaseHeight();
                    // wait();
                }
                if (touchX < 80) {
                    decreaseHeight();
                    // wait();
                }
            }
        }
    }
}

void mainScreenTickerFunc() {
    led != led;
    static int cnt = 1;
    speed = avg_speed * radius;
    dist += speed;

    lcd.SetFont(&Font24);
    // time elapsed; seconds
    sprintf((char*)text1, "%d", cnt);
    lcd.DisplayStringAt(0, 26, (uint8_t *)&text1, CENTER_MODE);
    // speed; km/h
    sprintf((char*)text2, "    %0.2f    ", speed * 3.6);
    lcd.DisplayStringAt(0, 124, (uint8_t *)&text2, CENTER_MODE);
    // distance; meters
    sprintf((char*)text3, "%0.2f", dist);
    lcd.DisplayStringAt(0, 224, (uint8_t *)&text3, CENTER_MODE); 
    cnt++;
}

void mainScreenView() {
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);

    // lcd.SetTextColor(LCD_COLOR_DARKMAGENTA);
    // lcd.FillEllipse(screenWidth / 2, 32, screenWidth / 4, screenWidth / 8);
    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 6, (uint8_t *)"TIME", CENTER_MODE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 48, (uint8_t *)"seconds", CENTER_MODE);
    
    // lcd.SetTextColor(LCD_COLOR_DARKGREEN);
    // lcd.FillRect((screenWidth / 4) - 20, 80, (screenWidth / 2) + 40, (screenWidth / 4) + 20);
    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 100, (uint8_t *)"SPEED", CENTER_MODE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 148, (uint8_t *)"KM / H", CENTER_MODE);
    
    // lcd.SetTextColor(LCD_COLOR_DARKCYAN);
    // lcd.FillEllipse(screenWidth / 2, 200, screenWidth / 4, screenWidth / 8);
    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.SetFont(&Font20);
    lcd.DisplayStringAt(0, 200, (uint8_t *)"DISTANCE", CENTER_MODE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 248, (uint8_t *)"METERS", CENTER_MODE);
    

    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, screenHeight - 32, (uint8_t *)"Tap to End", CENTER_MODE);
    
    
    // Gyro stuff

    Ticker tick;

    Gyroscope gyroscope(&spi, PC_1);
    int err = gyroscope.init();

    int16_t w_dps[3];
    int count = 0;
    
    button.fall(&Gyro_On);

    wait_us(1000000);

    tick.attach(&mainScreenTickerFunc, 1);
    
    while(1) {
        wait_us(100000);
        gyroscope.read_data_16(w_dps);
        
        gyroX = w_dps[0] * 0.00875f;
        gyroY = w_dps[1] * 0.00875f;
        gyroZ = w_dps[2] * 0.00875f;

        gyroX = gyroX + ZERO_RATE_NOISE;
        if (gyroX < 0.30) {
            gyroX = 0;
        }

        gyroX1 = gyroX * 0.0174533f;
        
        avg_speed = gyroscope.average_Velocity(gyroX1); //Circular queue to calculate average velocity
        //float x1 = x * 0.0174533f;s
        
        //printf("%04d | wx1: %f, wx: %f , temp: %d\n", count, x1,x, temp);
        printf("wx:%f  wx1:%f  temp:%f\n", gyroX, gyroX1, avg_speed);
        //printf("%04d | wx: %+7.2f dps, wy: %+7.2f dps, wz: %+7.2f dps\n", count, x, y, z);
        //led = !led;
        count++;

        // touchscreen stuff
        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        printf("x - %d, y - %d\n", touchX, touchY);

        if ((TS_State.TouchDetected) && (touchY < 40)) {
            activeScreen = RESULT_SCREEN;
            break;
        }
    }
}

void resultScreenView() {
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    
    // wait();

    lcd.SetTextColor(LCD_COLOR_DARKGREEN);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(0, 40, (uint8_t *)"Done!", CENTER_MODE);
    
    wait();

    lcd.SetTextColor(LCD_COLOR_BLUE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 100, (uint8_t *)"Distance Travelled", CENTER_MODE);
    wait();
    lcd.SetFont(&Font24);
    lcd.SetTextColor(LCD_COLOR_RED);
    sprintf((char*)text1, "%d meters", dist);
    lcd.DisplayStringAt(0, 125, (uint8_t *)&text1, CENTER_MODE);
    wait();
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_GREEN);
    lcd.DisplayStringAt(0, 180, (uint8_t *)"in", CENTER_MODE);
    wait();
    lcd.SetTextColor(LCD_COLOR_MAGENTA);
    sprintf((char*)text2, "%d seconds", avg_speed);
    lcd.DisplayStringAt(0, 200, (uint8_t *)&text2, CENTER_MODE);
    wait();

    lcd.SetTextColor(0xFF57068C);
    lcd.FillRect(screenMargin, screenHeight - 52, screenWidth - (2 * screenMargin), 44);
    lcd.SetBackColor(0xFF57068C);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, screenHeight - 36, (uint8_t *)"TRY AGAIN", CENTER_MODE);
    
    while(1) {        
        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        printf("x - %d, y - %d\n", touchX, touchY);

        if ((TS_State.TouchDetected) && (touchY < 40)) {
            activeScreen = SPLASH_SCREEN;
            break;
        }
    }
}

int main() {
    // init
    uint8_t status = ts.Init(lcd.GetXSize(), lcd.GetYSize());
  
    if (status != TS_OK) {
      lcd.Clear(LCD_COLOR_RED);
      lcd.SetBackColor(LCD_COLOR_RED);
      lcd.SetTextColor(LCD_COLOR_WHITE);
      lcd.SetFont(&Font24);
      lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN", CENTER_MODE);
      lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"INIT FAIL", CENTER_MODE);
    // } else {
    //   lcd.Clear(LCD_COLOR_GREEN);
    //   lcd.SetBackColor(LCD_COLOR_GREEN);
    //   lcd.SetTextColor(LCD_COLOR_WHITE);
    //   lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN", CENTER_MODE);
    //   lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"INIT SUCCESS", CENTER_MODE);
    }

    // wait();

    screenWidth = lcd.GetXSize();
    screenHeight = lcd.GetYSize();

    while(1) {
        switch (activeScreen) {
            case SPLASH_SCREEN:
                splashScreenView();
                break;
            case CONFIG_SCREEN:
                configScreenView();
                break;
            case MAIN_SCREEN:
                mainScreenView();
                break;
            case RESULT_SCREEN:
                resultScreenView();
                break;
            default:
                splashScreenView();
        }
    }

}
