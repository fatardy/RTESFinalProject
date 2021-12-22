/**
  ******************************************************************************
  * @file    LCD.cpp 
  * @author  Rituraj Singh, Aradhya Alamuru
  * @brief   Main File to interface with LCD(STM32F429I)
  *          This file provides API's to configure and interact with LCD 
  ******************************************************************************
  **/
// #include "GYRO_SENSOR.h"
#include <LCD.h>

LCD_DISCO_F429ZI lcd; // LCD Object
TS_DISCO_F429ZI ts; // Touch Screen Object

float distanceTravelled = 0;
uint32_t secondsCount = 1;
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
float inputUserHeight = 178.0; // default is my height
// ARC_length used to calculate linear velocity
float ARC_length;

// Show live graph or display text
bool showGraphView = false;
// Bool to exit graph view, cause it plots only 30s
bool exitGraphViewBool = false;

// For drawing live graph;
uint8_t screenMarginForGraph = 32;
uint8_t dividerHeightInGraph = 8;

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