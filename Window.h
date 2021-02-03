//
// Created by ardxwe on 2021/1/31.
//

#ifndef DEMO_WINDOW_H
#define DEMO_WINDOW_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <string>

class Window {
public:
  Window(const std::string &title, int x, int y, int w, int h, uint32_t flags);
  SDL_Window *get() const;

private:
  struct Deleter {
    void operator()(SDL_Window *x);
  };
  std::unique_ptr<SDL_Window, Deleter> window_;
};

#endif // DEMO_WINDOW_H
