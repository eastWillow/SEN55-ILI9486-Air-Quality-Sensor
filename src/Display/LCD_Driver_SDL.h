#ifndef LCD_DRIVER_SDL_H
#define LCD_DRIVER_SDL_H

#ifndef ARDUINO
// PC-specific declarations
void LCD_Update();
void LCD_SaveScreenshot(const char* filename);
void LCD_Quit();
#endif

#endif
