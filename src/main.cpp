#include <LCD.h>

int main() {

    LCD_Init();

    // Init with first screen;
    uint32_t activeScreen = SPLASH_SCREEN;
    
    while(1) {
        switch (activeScreen) {
            case SPLASH_SCREEN:
                activeScreen = splashScreenView();
                break;
            case CONFIG_SCREEN:
                activeScreen = configScreenView();
                break;
            case MAIN_SCREEN:
                activeScreen = mainScreenView();
                break;
            case RESULT_SCREEN:
                activeScreen = resultScreenView();
                break;
            default:
                activeScreen = splashScreenView();
        }
    }

}
