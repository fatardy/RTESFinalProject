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

extern uint32_t activeScreen;

// Initialize LCD with (x, y)
// Check if touchscreen initialized correctly. If not show error;
void LCD_Init();

void decreaseHeight();
void increaseHeight();

// Display the first screen UI on the LCD
// Once user taps the 'START' button, move to the next screen;
void splashScreenView();

// Initialize the second screen UI for the LCD
// Ask user to input height in centimeters;
// Once user taps 'GO', jump to the 3rd screen;
void configScreenView();

// Initialize the ticker called every one second;
// Displays the time elapsed, the speed and the distance travelled;
void mainScreenTickerFunc();

// Main screen UI used to display the time elapsed, speed,
// and the distance travelled by the user;
// Gyroscope is read in this functio at a 100Hz sampling rate;
// Speed and distance displayed using a ticker function
// Ticker attached to mainScreenTickerFunc;
void mainScreenView();

// Display the final screen: results screen;
// Once user stops and taps on 'DONE', display the 
// total distance travelled in time elapsed;
// 'TRY AGAIN' to jump back to the starting screen of the project;
void resultScreenView();

// Helper functions return screen height and width;
uint32_t getScreenWidth();
uint32_t getScreenHeight();

// Draw skeleton for the graph view;
void drawGraphViewSkeletonView();

extern LCD_DISCO_F429ZI lcd; // LCD Object
extern TS_DISCO_F429ZI ts; // Touch Screen Object

extern void wait();


// text arrays used for printing on lcd
extern uint8_t text1[30];
extern uint8_t text2[30];
extern uint8_t text3[30];

extern uint32_t screenMargin; // defined for ui
// Touchscreen state;
extern TS_StateTypeDef TS_State;
extern uint16_t touchX, touchY;
// NOTE:(0, 0) is at the bottom left; (x, y) also register the black part of the screen;

// User asked to set height;
extern float inputUserHeight; // default is my height
// ARC_length used to calculate linear velocity
extern float ARC_length;

// Show live graph or display text
extern bool showGraphView;
// Bool to exit graph view, cause it plots only 30s
extern bool exitGraphViewBool;

// For drawing live graph;
extern uint8_t screenMarginForGraph;
extern uint8_t dividerHeightInGraph;

// Helper function to return height and width of lcd;
extern uint32_t getScreenWidth();
extern uint32_t getScreenHeight();
// width is 240; height is 320;