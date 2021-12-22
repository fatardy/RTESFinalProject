/**
  ******************************************************************************
  * @file    LCD.h 
  * @author  Rituraj Singh, Aradhya Alamuru
  * @brief   Main File to interface with LCD(STM32F429I)
  *          This file provides API's to configure and interact with LCD 
  ******************************************************************************
  **/
#include "LCD_DISCO_F429ZI.h"
#include "TS_DISCO_F429ZI.h"

enum Screens {
    SPLASH_SCREEN = 1,
    CONFIG_SCREEN,
    MAIN_SCREEN,
    RESULT_SCREEN,
};

// Initialize LCD with (x, y)
// Check if touchscreen initialized correctly. If not show error;
void LCD_Init();

void decreaseHeight();
void increaseHeight();

// Display the first screen UI on the LCD
// Once user taps the 'START' button, move to the next screen;
uint32_t splashScreenView();

// Initialize the second screen UI for the LCD
// Ask user to input height in centimeters;
// Once user taps 'GO', jump to the 3rd screen;
uint32_t configScreenView();

// Initialize the ticker called every one second;
// Displays the time elapsed, the speed and the distance travelled;
void mainScreenTickerFunc();

// Main screen UI used to display the time elapsed, speed,
// and the distance travelled by the user;
// Gyroscope is read in this functio at a 100Hz sampling rate;
// Speed and distance displayed using a ticker function
// Ticker attached to mainScreenTickerFunc;
uint32_t mainScreenView();

// Display the final screen: results screen;
// Once user stops and taps on 'DONE', display the 
// total distance travelled in time elapsed;
// 'TRY AGAIN' to jump back to the starting screen of the project;
uint32_t resultScreenView();

// Helper functions return screen height and width;
uint32_t getScreenWidth();
uint32_t getScreenHeight();

// Draw skeleton for the graph view;
void drawGraphViewSkeletonView();