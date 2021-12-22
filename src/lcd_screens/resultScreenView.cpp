#include <LCD.h>
#include <GYRO_SENSOR.h>

// Display the final screen: results screen;
// Once user stops and taps on 'DONE', display the 
// total distance travelled in time elapsed;
// 'TRY AGAIN' to jump back to the starting screen of the project;
void resultScreenView() {
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
    sprintf((char*)text1, "%0.2f meters", distanceTravelled);
    lcd.DisplayStringAt(0, 137, (uint8_t *)&text1, CENTER_MODE);
    wait();
    lcd.SetFont(&Font16);
    lcd.SetTextColor(LCD_COLOR_BROWN);
    lcd.DisplayStringAt(0, 190, (uint8_t *)"in", CENTER_MODE);
    lcd.SetTextColor(LCD_COLOR_MAGENTA);
    sprintf((char*)text2, "%d seconds", secondsCount);
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
}