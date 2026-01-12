#include "SDL.h"
#include <stdio.h>

extern "C" {
int SDL_Init(uint32_t flags) { return 0; }
const char* SDL_GetError(void) { return "Mock Error"; }
SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, uint32_t flags) { return (SDL_Window*)1; }
SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, uint32_t flags) { return (SDL_Renderer*)1; }
SDL_Texture* SDL_CreateTexture(SDL_Renderer* renderer, uint32_t format, int access, int w, int h) { return (SDL_Texture*)1; }
int SDL_UpdateTexture(SDL_Texture* texture, const void* rect, const void* pixels, int pitch) { return 0; }
int SDL_RenderClear(SDL_Renderer* renderer) { return 0; }
int SDL_RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, const void* srcrect, const void* dstrect) { return 0; }
void SDL_RenderPresent(SDL_Renderer* renderer) {}
void SDL_DestroyTexture(SDL_Texture* texture) {}
void SDL_DestroyRenderer(SDL_Renderer* renderer) {}
void SDL_DestroyWindow(SDL_Window* window) {}
void SDL_Quit(void) {}
int SDL_PollEvent(SDL_Event* event) { return 0; }
void SDL_Delay(uint32_t ms) {}
SDL_Surface* SDL_CreateRGBSurfaceWithFormatFrom(void* pixels, int width, int height, int depth, int pitch, uint32_t format) { return (SDL_Surface*)1; }
int SDL_SaveBMP(SDL_Surface* surface, const char* file) {
    printf("Mock SDL_SaveBMP to %s\n", file);
    // Create a dummy file
    FILE* f = fopen(file, "w");
    if (f) { fprintf(f, "BMP"); fclose(f); }
    return 0;
}
void SDL_FreeSurface(SDL_Surface* surface) {}
SDL_bool SDL_SetHint(const char* name, const char* value) { return SDL_TRUE; }
}
