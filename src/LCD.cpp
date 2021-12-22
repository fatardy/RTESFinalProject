/**
  ******************************************************************************
  * @file    LCD.cpp 
  * @author  Rituraj Singh, Aradhya Alamuru
  * @brief   Main File to interface with LCD(STM32F429I)
  *          This file provides API's to configure and interact with LCD 
  ******************************************************************************
  **/
#include "GYRO_SENSOR.h"
#include "LCD.h"

//Gyroscope value below this will be discarded for calculating average velocity
#define ZERO_RATE_NOISE 0.280000

LCD_DISCO_F429ZI lcd; // LCD Object
TS_DISCO_F429ZI ts; // Touch Screen Object

SPI spi(PF_9, PF_8, PF_7);
DigitalOut cs(PC_1);

InterruptIn button(USER_BUTTON); // Button to reset Gyroscope and LCD
Ticker tick;
Gyroscope gyroscope(&spi, PC_1);

float speed = 0;
float dist = 0;
float gyroX;
float gyroY;
float gyroZ;
float gyroX1;
float avg_speed = 0;
bool debug_flag = false;
uint32_t cnt = 1;
// text arrays used for printing on lcd
uint8_t text1[30];
uint8_t text2[30];
uint8_t text3[30];

// width is 240; height is 320;
uint32_t screenMargin = 16; // defined for ui

TS_StateTypeDef TS_State;
uint16_t touchX, touchY;
// NOTE:(0, 0) is at the bottom left; (x, y) also register the black part of the screen;

// User asked to set height;
float userHeight = 178.0; // default is my height
// ARC_length used to calculate linear velocity
float ARC_length;

// Show live graph or display text
bool showGraphView = false;
// Bool to exit graph view, cause it plots only 30s
bool exitGraphViewBool = false;

// For drawing live graph;
uint32_t screenMarginForGraph = 32;
int dividerHeightInGraph = 8;


void wait() {
    wait_us(1000000);
}

uint32_t getScreenWidth(){
    return lcd.GetXSize();
}
uint32_t getScreenHeight(){
    return lcd.GetYSize();
}

// Initialize LCD with (x, y)
// Check if touchscreen initialized correctly. If not show error;
void LCD_Init(){
    uint8_t status = ts.Init(lcd.GetXSize(), lcd.GetYSize());
  
    if (status != TS_OK) {
      lcd.Clear(LCD_COLOR_RED);
      lcd.SetBackColor(LCD_COLOR_RED);
      lcd.SetTextColor(LCD_COLOR_WHITE);
      lcd.SetFont(&Font24);
      lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN", CENTER_MODE);
      lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"INIT FAIL", CENTER_MODE);
    } else {
        // Touchscreen init success;
    }
}

// Display the first screen UI on the LCD
// Once user taps the 'START' button, move to the next screen;
uint32_t splashScreenView() {
    uint32_t activeScreen;
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
    lcd.DrawLine(getScreenWidth() / 4, 134, (getScreenWidth() / 4) * 3, 134);
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_BLUE);
    lcd.DisplayStringAt(0, LINE(9), (uint8_t *)"Rituraj Singh", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"Aradhya Alamuru", CENTER_MODE);

    wait();
    
    lcd.SetTextColor(LCD_COLOR_LIGHTBLUE);
    lcd.SetBackColor(LCD_COLOR_LIGHTBLUE);
    lcd.FillRect(screenMargin, getScreenHeight() - 52, getScreenWidth() - (2 * screenMargin), 44);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, getScreenHeight() - 36, (uint8_t *)"START", CENTER_MODE);

    while(1) {
    
        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        //printf("x - %d, y - %d\n", touchX, touchY);

        if ((TS_State.TouchDetected) && (touchY < 40)) {
            activeScreen = CONFIG_SCREEN;
            break;
        }
    
    }
    return activeScreen;
}

void increaseHeight() {
    userHeight++;
}
void decreaseHeight() {
    userHeight--;
}

// Initialize the second screen UI for the LCD
// Ask user to input height in centimeters;
// Once user taps 'GO', jump to the 3rd screen;
uint32_t configScreenView() {
    uint32_t activeScreen;
    userHeight = 178;
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

    lcd.SetTextColor(LCD_COLOR_DARKCYAN);
    lcd.SetBackColor(LCD_COLOR_DARKCYAN);
    lcd.FillRect(screenMargin, getScreenHeight() - 108, getScreenWidth() - (2 * screenMargin), 44);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, getScreenHeight() - 90, (uint8_t *)"GO!", CENTER_MODE);

    lcd.SetTextColor(LCD_COLOR_DARKMAGENTA);
    lcd.SetBackColor(LCD_COLOR_DARKMAGENTA);
    lcd.FillRect(screenMargin, getScreenHeight() - 52, getScreenWidth() - (2 * screenMargin), 44);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, getScreenHeight() - 36, (uint8_t *)"Live Graph!", CENTER_MODE);
    
    lcd.SetTextColor(LCD_COLOR_DARKRED);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font24);
        
    while(1) {
        sprintf((char*)text1, " <  %0.1f  > ", userHeight);
        lcd.DisplayStringAt(0, 145, (uint8_t *)&text1, CENTER_MODE);

        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        printf("x - %d, y - %d\n", touchX, touchY);

        if (TS_State.TouchDetected) {
            if (touchY < 30) {
                activeScreen = MAIN_SCREEN;
                showGraphView = true;
                break;
            }
            if ((touchY > 30) && (touchY < 90)) {
                activeScreen = MAIN_SCREEN;
                showGraphView = false;
                break;
            }
            if ((touchY > 105) && (touchY < 190)) {
                if (touchX > 160) {
                    increaseHeight();
                    // adding delay so that the UI doesn't update too quickly;
                    wait_us(100000);
                }
                if (touchX < 80) {
                    decreaseHeight();
                    // adding delay so that the UI doesn't update too quickly;
                    wait_us(100000);
                }
            }
        }
    }
    return activeScreen;
}


// Draw the graph skeleton for the main screen graph view;
void drawGraphViewSkeletonView() {
    lcd.Clear(LCD_COLOR_WHITE);

    lcd.SetTextColor(LCD_COLOR_MAGENTA);
    lcd.DrawHLine(screenMarginForGraph, getScreenHeight() - screenMarginForGraph, getScreenWidth() - (2 * screenMarginForGraph));
    lcd.DrawVLine(screenMarginForGraph, screenMarginForGraph, getScreenHeight() - (2 * screenMarginForGraph));

    // lcd.SetTextColor(LCD_COLOR_ORANGE);
    // lcd.DrawVLine(screenMarginForGraph + ((screenMarginForGraph * cnt) / 5), (getScreenHeight() - screenMarginForGraph) - (dividerHeightInGraph / 2), dividerHeightInGraph);

    // draw dividers on the y axis;
    for (int x = 0; x < 26; x++) {
        lcd.DrawHLine(screenMarginForGraph - (dividerHeightInGraph / 2), screenMarginForGraph + (x * 10), dividerHeightInGraph);        
    }
    lcd.SetTextColor(LCD_COLOR_DARKYELLOW);
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(10, getScreenHeight() - 36, (uint8_t *)" 0", LEFT_MODE);
    lcd.DisplayStringAt(10, getScreenHeight() - 84, (uint8_t *)" 5", LEFT_MODE);
    lcd.DisplayStringAt(10, getScreenHeight() - 134, (uint8_t *)"10", LEFT_MODE);
    lcd.DisplayStringAt(10, getScreenHeight() - 184, (uint8_t *)"15", LEFT_MODE);
    lcd.DisplayStringAt(10, getScreenHeight() - 232, (uint8_t *)"20", LEFT_MODE);
    
    lcd.SetTextColor(LCD_COLOR_DARKGREEN);
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(screenMargin, screenMargin, (uint8_t *)"Speed", LEFT_MODE);
    lcd.SetTextColor(LCD_COLOR_DARKCYAN);
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(0, getScreenHeight() - screenMargin - 4, (uint8_t *)"Time", CENTER_MODE);

    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.SetFont(&Font20);
    lcd.DisplayStringAt(0, 16, (uint8_t *)"Live Graph", RIGHT_MODE);
    
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(0, 60, (uint8_t *)"meters", RIGHT_MODE);

    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(0, 100, (uint8_t *)"km/h", RIGHT_MODE);

    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font12);
    lcd.DisplayStringAt(0, 140, (uint8_t *)"seconds", RIGHT_MODE);

    lcd.SetTextColor(LCD_COLOR_GRAY);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 170, (uint8_t *)"Exit>", RIGHT_MODE);
}

// Initialize the ticker called every one second;
// Displays the time elapsed, the speed and the distance travelled;
void mainScreenTickerFunc() {
    debug_flag = true;
    speed = avg_speed * ARC_length;
    dist += speed;

    float printSpeed = speed * 3.6;

    if (showGraphView) {
        // redraw graph every 30 seconds;
        uint32_t modCnt = cnt % 30;

        if (modCnt == 0) {
            // clear the graph view;
            drawGraphViewSkeletonView();
        }

        lcd.SetTextColor(LCD_COLOR_RED);
        // height is multiplied by 10
        // width is divided by 2
        lcd.FillCircle(screenMarginForGraph + ((screenMarginForGraph * modCnt) / 5), (getScreenHeight() - screenMarginForGraph) - (10 * printSpeed), 3);
        lcd.SetTextColor(LCD_COLOR_LIGHTRED);
        lcd.DrawVLine(screenMarginForGraph + ((screenMarginForGraph * modCnt) / 5), (getScreenHeight() - screenMarginForGraph) - (10 * printSpeed), 10 * printSpeed);
        
        // x axis divider lines; Unused; doesn't look very good;
        // lcd.SetTextColor(LCD_COLOR_DARKGREEN);
        // lcd.DrawVLine(screenMarginForGraph + ((screenMarginForGraph * modCnt) / 5), (getScreenHeight() - screenMarginForGraph) - (dividerHeightInGraph / 2), dividerHeightInGraph);

        // Display numbers on the x axis; Unused;
        // lcd.SetTextColor(LCD_COLOR_DARKBLUE);
        // lcd.SetFont(&Font12);
        // sprintf((char*)text1, "%d", modCnt);
        // lcd.DisplayStringAt(screenMarginForGraph + ((screenMarginForGraph * modCnt) / 5), getScreenHeight() - screenMargin, (uint8_t *)&text1, LEFT_MODE);

        lcd.SetTextColor(LCD_COLOR_RED);
        lcd.SetFont(&Font20);
        sprintf((char*)text3, "%0.2f", dist);
        lcd.DisplayStringAt(0, 40, (uint8_t *)&text3, RIGHT_MODE); 

        lcd.SetTextColor(LCD_COLOR_DARKGREEN);
        lcd.SetFont(&Font20);
        sprintf((char*)text2, "   %0.2f", printSpeed);
        lcd.DisplayStringAt(0, 80, (uint8_t *)&text2, RIGHT_MODE);

        lcd.SetTextColor(LCD_COLOR_DARKCYAN);
        lcd.SetFont(&Font20);
        sprintf((char*)text1, "%d", cnt);
        lcd.DisplayStringAt(0, 120, (uint8_t *)&text1, RIGHT_MODE);
        
    } else {
        lcd.SetFont(&Font24);
        // time elapsed; seconds
        sprintf((char*)text1, "%d", cnt);
        lcd.DisplayStringAt(0, 26, (uint8_t *)&text1, CENTER_MODE);
        // speed; km/hr
        sprintf((char*)text2, "    %0.2f    ", printSpeed);
        lcd.DisplayStringAt(0, 124, (uint8_t *)&text2, CENTER_MODE);
        // distance; meters
        sprintf((char*)text3, "%0.2f", dist);
        lcd.DisplayStringAt(0, 224, (uint8_t *)&text3, CENTER_MODE); 
    }

    cnt++;
}


// Main screen UI used to display the time elapsed, speed,
// and the distance travelled by the user;
// Gyroscope is read in this functio at a 100Hz sampling rate;
// Speed and distance displayed using a ticker function
// Ticker attached to mainScreenTickerFunc;
uint32_t mainScreenView() {
    uint32_t activeScreen;
    
    speed = 0;
    dist = 0;
    avg_speed = 0;
    cnt = 1;
    
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);

    if (showGraphView) {
        drawGraphViewSkeletonView();
    } else {
        lcd.SetTextColor(LCD_COLOR_RED);
        lcd.SetFont(&Font16);
        lcd.DisplayStringAt(0, 6, (uint8_t *)"TIME", CENTER_MODE);
        lcd.SetFont(&Font16);
        lcd.DisplayStringAt(0, 48, (uint8_t *)"seconds", CENTER_MODE);
        
        // lcd.SetTextColor(LCD_COLOR_DARKGREEN);
        // lcd.FillRect((getScreenWidth() / 4) - 20, 80, (getScreenWidth() / 2) + 40, (getScreenWidth() / 4) + 20);
        lcd.SetTextColor(LCD_COLOR_RED);
        lcd.SetFont(&Font16);
        lcd.DisplayStringAt(0, 100, (uint8_t *)"SPEED", CENTER_MODE);
        lcd.SetFont(&Font16);
        lcd.DisplayStringAt(0, 148, (uint8_t *)"KM / H", CENTER_MODE);
        
        // lcd.SetTextColor(LCD_COLOR_DARKCYAN);
        // lcd.FillEllipse(getScreenWidth() / 2, 200, getScreenWidth() / 4, getScreenWidth() / 8);
        lcd.SetTextColor(LCD_COLOR_RED);
        lcd.SetFont(&Font20);
        lcd.DisplayStringAt(0, 200, (uint8_t *)"DISTANCE", CENTER_MODE);
        lcd.SetFont(&Font16);
        lcd.DisplayStringAt(0, 248, (uint8_t *)"METERS", CENTER_MODE);
        
        lcd.SetTextColor(LCD_COLOR_BLACK);
        lcd.SetFont(&Font16);
        lcd.DisplayStringAt(0, getScreenHeight() - 32, (uint8_t *)"Tap to End", CENTER_MODE);
    }
    
    int err = gyroscope.init();

    int16_t w_dps[3];
    int count = 0;
    
    wait();

    ARC_length = userHeight/200;
    tick.attach(&mainScreenTickerFunc, 1.0);
    
    while(1) {
        wait_us(100000);
        gyroscope.read_data_16(w_dps);
        
        gyroX = w_dps[0] * 0.00875f; // Calculate average linear velocity and distance;
        gyroY = w_dps[1] * 0.00875f; // Unused;
        gyroZ = w_dps[2] * 0.00875f; // Unused;

        gyroX = gyroX + ZERO_RATE_NOISE;
        if (gyroX < 0.30) {
            gyroX = 0;  
        }

        gyroX1 = gyroX * 0.0174533f;
        
        // Circular queue to calculate the average velocity;
        avg_speed = gyroscope.average_Velocity(gyroX1);

        if(debug_flag){
            debug_flag = false;
            printf("%0.2f  ",avg_speed*ARC_length);
        }
        //printf("wx:%f  wx1:%f  temp:%f\n", gyroX, gyroX1, avg_speed);

        count++;

        // touchscreen stuff
        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        // printf("x - %d, y - %d\n", touchX, touchY);

        if (TS_State.TouchDetected) {
            activeScreen = RESULT_SCREEN;

            if ((!showGraphView) && (touchY < 40)) {
                break;
            }
            if (showGraphView && (touchY < 170) && (touchY > 70) && (touchX > 150)) {
                break;
            }
        }

        if (exitGraphViewBool) {
            exitGraphViewBool = false;
            activeScreen = RESULT_SCREEN;
            break;
        }
    }

    tick.detach();
    return activeScreen;
}


// Display the final screen: results screen;
// Once user stops and taps on 'DONE', display the 
// total distance travelled in time elapsed;
// 'TRY AGAIN' to jump back to the starting screen of the project;
uint32_t resultScreenView() {
    uint32_t activeScreen;
    wait();
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    
    wait();

    lcd.SetTextColor(LCD_COLOR_DARKGREEN);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(0, 40, (uint8_t *)"Done!", CENTER_MODE);

    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.DrawLine(getScreenWidth() / 4, 72, (getScreenWidth() / 4) * 3, 72);
    
    wait();

    lcd.SetTextColor(LCD_COLOR_BLUE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, 100, (uint8_t *)"Distance", CENTER_MODE);
    lcd.DisplayStringAt(0, 120, (uint8_t *)"Travelled", CENTER_MODE);
    wait();
    lcd.SetFont(&Font24);
    lcd.SetTextColor(LCD_COLOR_RED);
    sprintf((char*)text1, "%0.2f meters", dist);
    lcd.DisplayStringAt(0, 137, (uint8_t *)&text1, CENTER_MODE);
    wait();
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_BROWN);
    lcd.DisplayStringAt(0, 190, (uint8_t *)"in", CENTER_MODE);
    lcd.SetTextColor(LCD_COLOR_MAGENTA);
    sprintf((char*)text2, "%d seconds", cnt);
    lcd.DisplayStringAt(0, 205, (uint8_t *)&text2, CENTER_MODE);
    wait();

    lcd.SetTextColor(0xFF57068C);
    lcd.FillRect(screenMargin, getScreenHeight() - 52, getScreenWidth() - (2 * screenMargin), 44);
    lcd.SetBackColor(0xFF57068C);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, getScreenHeight() - 36, (uint8_t *)"TRY AGAIN", CENTER_MODE);
    
    while(1) {        
        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        //printf("x - %d, y - %d\n", touchX, touchY);

        if ((TS_State.TouchDetected) && (touchY < 40)) {
            activeScreen = SPLASH_SCREEN;
            break;
        }
    }
    return activeScreen;
}