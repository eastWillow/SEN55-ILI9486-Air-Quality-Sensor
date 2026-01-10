// Mock SDL2 header for compilation check only in this environment
#ifndef SDL_H
#define SDL_H

#include <stdint.h>

#define SDL_INIT_VIDEO 0
#define SDL_WINDOWPOS_UNDEFINED 0
#define SDL_WINDOW_SHOWN 0
#define SDL_RENDERER_ACCELERATED 0
#define SDL_PIXELFORMAT_RGB565 0
#define SDL_TEXTUREACCESS_STREAMING 0
#define SDL_QUIT 0

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Surface SDL_Surface;

typedef struct SDL_Event {
    uint32_t type;
} SDL_Event;

extern "C" {
int SDL_Init(uint32_t flags);
const char* SDL_GetError(void);
SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, uint32_t flags);
SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, uint32_t flags);
SDL_Texture* SDL_CreateTexture(SDL_Renderer* renderer, uint32_t format, int access, int w, int h);
int SDL_UpdateTexture(SDL_Texture* texture, const void* rect, const void* pixels, int pitch);
int SDL_RenderClear(SDL_Renderer* renderer);
int SDL_RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, const void* srcrect, const void* dstrect);
void SDL_RenderPresent(SDL_Renderer* renderer);
void SDL_DestroyTexture(SDL_Texture* texture);
void SDL_DestroyRenderer(SDL_Renderer* renderer);
void SDL_DestroyWindow(SDL_Window* window);
void SDL_Quit(void);
int SDL_PollEvent(SDL_Event* event);
void SDL_Delay(uint32_t ms);
SDL_Surface* SDL_CreateRGBSurfaceWithFormatFrom(void* pixels, int width, int height, int depth, int pitch, uint32_t format);
int SDL_SaveBMP(SDL_Surface* surface, const char* file);
void SDL_FreeSurface(SDL_Surface* surface);
}

#endif
