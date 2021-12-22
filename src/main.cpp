#include <LCD.h>

uint32_t activeScreen;

int main() {

    LCD_Init();

    // Init with first screen;
    activeScreen = SPLASH_SCREEN;
    
    while(1) {
        switch (activeScreen) {
            case SPLASH_SCREEN:
                splashScreenView();
                break;
            case CONFIG_SCREEN:
                configScreenView();
                break;
            case MAIN_SCREEN:
                mainScreenView();
                break;
            case RESULT_SCREEN:
                resultScreenView();
                break;
            default:
                splashScreenView();
        }
    }

}
