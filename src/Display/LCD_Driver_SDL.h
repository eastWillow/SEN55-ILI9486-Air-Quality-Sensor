#ifndef LCD_DRIVER_SDL_H
#define LCD_DRIVER_SDL_H

#ifndef ARDUINO
// PC-specific declarations
void LCD_Update();
void LCD_SaveScreenshot(const char* filename);
void LCD_Quit();

// Mouse simulation functions
void SDL_SetMouseState(int x, int y, bool pressed);
void SDL_GetMouseState_Mock(int *x, int *y, bool *pressed);

#endif

#endif
