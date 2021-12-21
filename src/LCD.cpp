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

LCD_DISCO_F429ZI lcd;                                           //LCD object
TS_DISCO_F429ZI ts;                                             //Touch screen object

DigitalOut led(LED2);                                           //LED object
SPI spi(PF_9, PF_8, PF_7);                                      //SPI object
DigitalOut cs(PC_1);

InterruptIn button(USER_BUTTON);                                //Button for resetting the LCD and Gyroscope
Ticker tick;                                                    //Ticker object
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
uint8_t text1[30];
uint8_t text2[30];
uint8_t text3[30];


// width is 240; height is 320;
uint32_t screenMargin = 16; // defined for ui

TS_StateTypeDef TS_State;
uint16_t touchX, touchY;
// NOTE:(0, 0) is at the bottom left; (x, y) also register the black part of the screen;

//User will be asked to set userHeight
float userHeight;
//ARC_length will be used to calculate linear velocity
float ARC_length;

void wait() {
    wait_us(1000000);
}

uint32_t get_screenWidth(){
    return lcd.GetXSize();
}
uint32_t get_screenHeight(){
    return lcd.GetYSize();
}

//This function will initialize the LCD with the (x,y)
//Background color, text color and font size
void LCD_Init(){
    uint8_t status = ts.Init(lcd.GetXSize(), lcd.GetYSize());
  
    if (status != TS_OK) {
      lcd.Clear(LCD_COLOR_RED);
      lcd.SetBackColor(LCD_COLOR_RED);
      lcd.SetTextColor(LCD_COLOR_WHITE);
      lcd.SetFont(&Font24);
      lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN", CENTER_MODE);
      lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"INIT FAIL", CENTER_MODE);
    }
}

//This function will display the first screen of the device
//Once the user touches the 'START' button,it will move to 2nd screen
uint32_t splashScreenView(uint32_t screenWidth,uint32_t screenHeight) {
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
    lcd.DrawLine(screenWidth / 4, 134, (screenWidth / 4) * 3, 134);
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_BLUE);
    lcd.DisplayStringAt(0, LINE(9), (uint8_t *)"Rituraj Singh", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"Aradhya Alamuru", CENTER_MODE);

    wait();
    
    lcd.SetTextColor(LCD_COLOR_LIGHTBLUE);
    lcd.SetBackColor(LCD_COLOR_LIGHTBLUE);
    lcd.FillRect(screenMargin, screenHeight - 52, screenWidth - (2 * screenMargin), 44);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, screenHeight - 36, (uint8_t *)"START", CENTER_MODE);

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

//This function is used to initialize 2nd Screen for the device
//IT will ask user for the height(in cm)
//Once user selects the height an press 'GO' button , it will jump to 3rd screen
uint32_t configScreenView(uint32_t screenWidth,uint32_t screenHeight) {
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
        sprintf((char*)text1, " <  %0.1f  > ", userHeight);
        lcd.DisplayStringAt(0, 170, (uint8_t *)&text1, CENTER_MODE);

        ts.GetState(&TS_State);
        touchX = TS_State.X;
        touchY = TS_State.Y;

        //printf("x - %d, y - %d\n", touchX, touchY);

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
    return activeScreen;
}

//This function is attached in a ticker to be called after every 1 second
//This function will display Time elapsed, Speed and distance travelled
void mainScreenTickerFunc() {
    debug_flag = true;
    led != led;
    speed = avg_speed * ARC_length;
    dist += speed;

    lcd.SetFont(&Font24);
    // time elapsed; seconds
    sprintf((char*)text1, "%d", cnt);
    lcd.DisplayStringAt(0, 26, (uint8_t *)&text1, CENTER_MODE);
    // speed; km/hr
    sprintf((char*)text2, "    %0.2f    ", speed * 3.6);
    lcd.DisplayStringAt(0, 124, (uint8_t *)&text2, CENTER_MODE);
    // distance; meters
    sprintf((char*)text3, "%0.2f", dist);
    lcd.DisplayStringAt(0, 224, (uint8_t *)&text3, CENTER_MODE); 
    cnt++;
}

//This is the 3rd and main screen which will display time elapsed,
//Speed and distance travelled by the user
//Gyroscope is read in this function at a 100HZ sampling rate
//Speed and distance are displayed using a ticker function
//Ticker is attached to mainScreenTickerFunc
uint32_t mainScreenView(uint32_t screenWidth,uint32_t screenHeight) {
    uint32_t activeScreen;
    
    speed = 0;
    dist = 0;
    avg_speed = 0;
    cnt = 1;

    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);

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
 
    int err = gyroscope.init();

    int16_t w_dps[3];
    int count = 0;
    
   // button.fall(&Gyro_On);

    wait_us(1000000);

    ARC_length = userHeight/200;
    tick.attach(&mainScreenTickerFunc, 1.0);
    
    while(1) {
        wait_us(100000);
        gyroscope.read_data_16(w_dps);
        
        gyroX = w_dps[0] * 0.00875f;                    //Used for calculating the average linear velocity and distance
        gyroY = w_dps[1] * 0.00875f;                    //Not used for this project
        gyroZ = w_dps[2] * 0.00875f;                    //Not used for this project

        gyroX = gyroX + ZERO_RATE_NOISE;
        if (gyroX < 0.30) {
            gyroX = 0;  
        }

        gyroX1 = gyroX * 0.0174533f;
        
        avg_speed = gyroscope.average_Velocity(gyroX1); //Circular queue to calculate average velocity

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

        //printf("x - %d, y - %d\n", touchX, touchY);

        if ((TS_State.TouchDetected) && (touchY < 40)) {
            activeScreen = RESULT_SCREEN;
            break;
        }
    }
    tick.detach();
    return activeScreen;
}

//This function is used to display the 4th and the final screen
//Once the user stops and press Done button, it will display total distance travelled 
//in complete journey with total time
//If user Presses "TRY AGAIN", it will jum to starting screen of the device and Device is reset
uint32_t resultScreenView(uint32_t screenWidth,uint32_t screenHeight) {
    uint32_t activeScreen;
    wait();
    lcd.Clear(LCD_COLOR_WHITE);
    lcd.SetBackColor(LCD_COLOR_WHITE);
    
    wait();

    lcd.SetTextColor(LCD_COLOR_DARKGREEN);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(0, 40, (uint8_t *)"Done!", CENTER_MODE);

    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.DrawLine(screenWidth / 4, 72, (screenWidth / 4) * 3, 72);
    
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
    lcd.FillRect(screenMargin, screenHeight - 52, screenWidth - (2 * screenMargin), 44);
    lcd.SetBackColor(0xFF57068C);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);
    lcd.DisplayStringAt(0, screenHeight - 36, (uint8_t *)"TRY AGAIN", CENTER_MODE);
    
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

