#include <LCD.h>

// Display the first screen UI on the LCD
// Once user taps the 'START' button, move to the next screen;
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
}
