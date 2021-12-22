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

//This function will initialize the LCD with the (x,y)
//Background color, text color and font size
void LCD_Init();

void decreaseHeight();
void increaseHeight();

//This function will display the first screen of the device
//Once the user touches the 'START' button,it will move to 2nd screen
uint32_t splashScreenView();

//This function is used to initialize 2nd Screen for the device
//IT will ask user for the height(in cm)
//Once user selects the height an press 'GO' button , it will jump to 3rd screen
uint32_t configScreenView();

//This function is attached in a ticker to be called after every 1 second
//This function will display Time elapsed, Speed and distance travelled
void mainScreenTickerFunc();

//This is the 3rd and main screen which will display time elapsed,
//Speed and distance travelled by the user
//Gyroscope is read in this function at a 100HZ sampling rate
//Speed and distance are displayed using a ticker function
//Ticker is attached to mainScreenTickerFunc
uint32_t mainScreenView();

//This function is used to display the 4th and the final screen
//Once the user stops and press Done button, it will display total distance travelled 
//in complete journey with total time
//If user Presses "TRY AGAIN", it will jum to starting screen of the device and Device is reset
uint32_t resultScreenView();

//Functions to hold the screen width and Height 
uint32_t getScreenWidth();
uint32_t getScreenHeight();

