//
// Created by ardxwe on 2021/1/31.
//

#include "Window.h"

#include <SDL2/SDL.h>
#include <stdexcept>
#include <string>

Window::Window(const std::string &title, int x, int y, int w, int h,
               uint32_t flags)
    : window_{SDL_CreateWindow(title.c_str(), x, y, w, h, flags)} {
  if (window_ == nullptr) {
    using namespace std::string_literals;
    throw std::runtime_error{"error call SDL_CreateWindow"s + SDL_GetError()};
  }
}

SDL_Window *Window::get() const { return window_.get(); }

void Window::Deleter::operator()(SDL_Window *x) { SDL_DestroyWindow(x); }
