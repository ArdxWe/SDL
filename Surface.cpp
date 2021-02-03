//
// Created by ardxwe on 2021/2/3.
//

#include "Surface.h"
#include "Window.h"

#include <SDL2/SDL.h>

Surface::Surface(const Window &window)
    : surface_{SDL_GetWindowSurface(window.get())} {}

void Surface::fill_rect(SDL_Rect *rect, uint32_t color) {
  SDL_FillRect(surface_, rect, color);
}

SDL_PixelFormat *Surface::get_format() { return (surface_)->format; }
