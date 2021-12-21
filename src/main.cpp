#include <LCD.h>

int main() {

    // init
    LCD_Init();

    uint32_t screenWidth = get_screenWidth();                                   //Screen width
    uint32_t screenHeight = get_screenHeight();                                 //Screen Height
    uint32_t activeScreen = SPLASH_SCREEN;                                      //Active Screen initialized with 1st
    
    while(1) {
        switch (activeScreen) {
            case SPLASH_SCREEN:
                activeScreen = splashScreenView(screenWidth,screenHeight);
                break;
            case CONFIG_SCREEN:
                activeScreen = configScreenView(screenWidth,screenHeight);
                break;
            case MAIN_SCREEN:
                activeScreen = mainScreenView(screenWidth,screenHeight);
                break;
            case RESULT_SCREEN:
                activeScreen = resultScreenView(screenWidth,screenHeight);
                break;
            default:
                activeScreen = splashScreenView(screenWidth,screenHeight);
        }
    }

}
