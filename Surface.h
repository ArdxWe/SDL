//
// Created by ardxwe on 2021/2/3.
//

#ifndef DEMO_SURFACE_H
#define DEMO_SURFACE_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>

class Window;
class Surface {
public:
  Surface(const Window &window);
  void fill_rect(SDL_Rect *rect, uint32_t color);
  SDL_PixelFormat *get_format();

private:
  SDL_Surface *surface_;
};

#endif //DEMO_SURFACE_H
