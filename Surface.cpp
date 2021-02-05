//
// Created by ardxwe on 2021/2/3.
//

#include "Surface.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>

using std::runtime_error;
using namespace std::string_literals;

Surface::Surface(const std::string &path) : surface_{IMG_Load(path.c_str())} {
  if (surface_ == nullptr) {
    throw runtime_error{"Error calling IMG_Load: "s + IMG_GetError()};
  }
}

SDL_PixelFormat *Surface::getFormat() { return surface_->format; }

SDL_Surface *Surface::get() { return surface_.get(); }

void Surface::Deleter::operator()(SDL_Surface *p) { SDL_FreeSurface(p); }
