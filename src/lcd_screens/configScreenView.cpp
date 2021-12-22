#include <LCD.h>

void increaseHeight() {
    inputUserHeight++;
}
void decreaseHeight() {
    inputUserHeight--;
}

// Initialize the second screen UI for the LCD
// Ask user to input height in centimeters;
// Once user taps 'GO', jump to the 3rd screen;
void configScreenView() {
    inputUserHeight = 178;
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
        sprintf((char*)text1, " <  %0.1f  > ", inputUserHeight);
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
}
