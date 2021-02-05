//
// Created by ardxwe on 2021/2/6.
//

#include "Application.h"

#include <chrono>
#include <cstdio>
#include <future>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::async;
using std::future;
using std::move;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;
using namespace std::string_literals;

constexpr double FADE_OUT_TIME = 2;
constexpr double FADE_IN_TIME = 2;
constexpr double ON_SHOW_TIME = 4;

stringstream executeCmd(const string &cmd) {
  auto close = [](FILE *file) { pclose(file); };
  unique_ptr<FILE, decltype(close)> pipe{popen(cmd.c_str(), "r")};

  vector<char> buff(0x100);
  size_t n;
  stringstream stream;

  while ((n = fread(buff.data(), sizeof(buff[0]), buff.size(), pipe.get())) >
         0) {
    stream.write(buff.data(), n);
  }
  return stream;
}
vector<string> getImagePaths() {
  stringstream stream = executeCmd("find ~/Pictures -name '*.png'"s);

  vector<string> res;
  string path;
  while (getline(stream, path)) {
    res.push_back(move(path));
  }
  if (res.size() < 4) {
    throw runtime_error{"less than four pictures."s};
  }
  return res;
}

Window createWindow() {
  string name{"my demo"};
  int width = 800, height = 600;
  Window window{name, 0x1FFF0000, 0x1FFF0000, width, height, SDL_WINDOW_SHOWN};
  return window;
}

Texture createTextureFromSurface(Renderer &renderer, Surface &surface) {
  return Texture{SDL_CreateTextureFromSurface(renderer.get(), surface.get())};
}

future<Surface> nextImage(const string &path) {
  return async(std::launch::async, [&]() { return Surface{path}; });
}

Application::Application()
    : paths_{getImagePaths()}, window_{createWindow()}, renderer_{window_},
      image_{nextImage(paths_[0]).get()}, next_image_{nextImage(paths_[1])} {
  renderer_.setColor(0xFF, 0xFF, 0xFF, 0xFF);
}

void Application::run() {
  bool quit = false;
  SDL_Event e;
  auto start = std::chrono::high_resolution_clock::now();
  auto now = start;
  int size = paths_.size(), i = 0;
  double alpha, tm;
  double time_long = FADE_IN_TIME;
  while (!quit) {

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    now = std::chrono::high_resolution_clock::now();
    tm = time_long -
         std::chrono::duration_cast<std::chrono::duration<double>>(now - start)
             .count();

    switch (state_) {
    case State::FADE_IN:
      alpha = 1 - tm / time_long;
      if (tm < 0) {
        state_ = State::ON_SHOW;
        time_long = ON_SHOW_TIME;
        start = std::chrono::high_resolution_clock::now();
      }
      break;
    case State::FADE_OUT:
      alpha = tm / time_long;
      if (tm < 0 && next_image_.wait_for((std::chrono::seconds)0) ==
                        std::future_status::ready) {
        state_ = State::FADE_IN;
        time_long = FADE_OUT_TIME;
        start = std::chrono::high_resolution_clock::now();
        i++;
        if (i == size) {
          i = 0;
        }
        image_ = move(next_image_.get());
        next_image_ = nextImage(paths_[i]);
      }
      break;
    case State::ON_SHOW:
      alpha = 1;
      if (tm < 0) {
        state_ = State::FADE_OUT;
        time_long = FADE_OUT_TIME;
        start = std::chrono::high_resolution_clock::now();
      }
      break;
    }
    if (alpha < 0)
      alpha = 0;
    if (alpha > 1)
      alpha = 1;
    current_texture_ = createTextureFromSurface(renderer_, image_);
    current_texture_.setBlendMode(SDL_BLENDMODE_BLEND);
    current_texture_.setAlpha(alpha * 255);
    renderer_.clear();
    renderer_.copyTexture(current_texture_);
    renderer_.renderPresent();
  }
}