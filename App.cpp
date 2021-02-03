//
// Created by ardxwe on 2021/1/31.
//

#include "App.h"

#include <SDL2/SDL.h>
#include <stdexcept>
#include <string>

using namespace std::string_literals;
using std::runtime_error;

App::App() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw runtime_error{"Error call SDL_Init"s + SDL_GetError()};
  }
}

App::~App() { SDL_Quit(); }
