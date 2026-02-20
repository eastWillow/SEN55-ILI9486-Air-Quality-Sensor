#ifndef ARDUINO
#include "LCD_Driver.h"
#include "LCD_Driver_SDL.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

// SDL Global Variables
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint16_t* frameBuffer = NULL; // 16-bit color buffer

// Mouse State
static int s_mouseX = 0;
static int s_mouseY = 0;
static bool s_mousePressed = false;

LCD_DIS sLCD_DIS = {LCD_WIDTH, LCD_HEIGHT, SCAN_DIR_DFT, 0, 0};

#define WINDOW_WIDTH  LCD_WIDTH
#define WINDOW_HEIGHT LCD_HEIGHT

// Helper to convert RGB565 to RGB888 for SDL
// RGB565: RRRRRGGG GGGBBBBB
// RGB888: RRRRRRRR GGGGGGGG BBBBBBBB
void RGB565_To_RGB888(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = (color & 0xF800) >> 8; // Top 5 bits
    *g = (color & 0x07E0) >> 3; // Middle 6 bits
    *b = (color & 0x001F) << 3; // Bottom 5 bits
    // Scale up to full 8-bit range (simple shift leaves gaps, but enough for testing)
    *r |= (*r >> 5);
    *g |= (*g >> 6);
    *b |= (*b >> 5);
}

// Convert RGB888 to RGB565 (if needed)
uint16_t RGB888_To_RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void LCD_Init(LCD_SCAN_DIR LCD_ScanDir, uint16_t LCD_BLval) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create window
    window = SDL_CreateWindow("SEN55 LCD Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create texture for the framebuffer
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (texture == NULL) {
        fprintf(stderr, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Allocate framebuffer
    size_t bufferSize = WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint16_t);
    frameBuffer = (uint16_t*)malloc(bufferSize);
    if (frameBuffer == NULL) {
        fprintf(stderr, "Framebuffer could not be allocated!\n");
        exit(1);
    }
    memset(frameBuffer, 0xFFFF, bufferSize); // White init
}

void LCD_SetGramScanWay(LCD_SCAN_DIR Scan_dir) {
    // SDL implementation might just ignore scan direction for simplicity,
    // or we can implement rotation if critical.
    // For this task, we assume standard orientation.
}

void LCD_WriteReg(uint8_t Reg) {
    // No-op for emulator
}

void LCD_WriteData(uint8_t Data) {
    // No-op for emulator
}

// Update the texture and render to screen
void LCD_Update() {
    SDL_UpdateTexture(texture, NULL, frameBuffer, WINDOW_WIDTH * sizeof(uint16_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void LCD_SetWindow(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend) {
    // No-op or store for optimization
}

void LCD_SetCursor(POINT Xpoint, POINT Ypoint) {
    // No-op
}

void LCD_SetColor(COLOR Color, POINT Xpoint, POINT Ypoint) {
    LCD_SetPointlColor(Xpoint, Ypoint, Color);
}

void LCD_SetPointlColor(POINT Xpoint, POINT Ypoint, COLOR Color) {
    if (Xpoint < WINDOW_WIDTH && Ypoint < WINDOW_HEIGHT) {
        frameBuffer[Ypoint * WINDOW_WIDTH + Xpoint] = Color;
    }
    // Note: To see changes immediately, we could call Update,
    // but usually it's better to update once per loop or after bulk ops.
    // However, the original code might expect immediate drawing.
    // For performance, we might want to defer, but let's try immediate if it's not too slow,
    // OR rely on the App loop to call a refresh (which isn't in the original API).
    // The original code doesn't have a "Flush" command.
    // So for emulation, we might just update periodically in the main loop of the PC app.
}

void LCD_SetArealColor(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend, COLOR Color) {
    for (int y = Ystart; y < Yend; y++) {
        for (int x = Xstart; x < Xend; x++) {
            LCD_SetPointlColor(x, y, Color);
        }
    }
}

void LCD_Clear(COLOR Color) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        frameBuffer[i] = Color;
    }
    LCD_Update();
}

// SDL Cleanup (to be called at end of main)
void LCD_Quit() {
    free(frameBuffer);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Screenshot function for PC
void LCD_SaveScreenshot(const char* filename) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(frameBuffer, WINDOW_WIDTH, WINDOW_HEIGHT, 16, WINDOW_WIDTH * 2, SDL_PIXELFORMAT_RGB565);
    SDL_SaveBMP(surface, filename);
    SDL_FreeSurface(surface);
}

void SDL_SetMouseState(int x, int y, bool pressed) {
    s_mouseX = x;
    s_mouseY = y;
    s_mousePressed = pressed;
}

void SDL_GetMouseState_Mock(int *x, int *y, bool *pressed) {
    if (x) *x = s_mouseX;
    if (y) *y = s_mouseY;
    if (pressed) *pressed = s_mousePressed;
}

#endif
