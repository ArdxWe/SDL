//
// Created by ardxwe on 2021/2/3.
//

#ifndef DEMO_SURFACE_H
#define DEMO_SURFACE_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <string>

class Window;
class Surface {
public:
  Surface(const std::string &path);
  void fill_rect(SDL_Rect *rect, uint32_t color);
  SDL_PixelFormat *getFormat();
  SDL_Surface *get();

private:
  struct Deleter {
    void operator()(SDL_Surface *p);
  };
  std::unique_ptr<SDL_Surface, Deleter> surface_;
};

#endif // DEMO_SURFACE_H
