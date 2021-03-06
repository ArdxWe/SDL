//
// Created by ardxwe on 2021/2/6.
//

#include "Application.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using std::array;
using std::getenv;
using std::move;
using std::runtime_error;
using std::sort;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;
using Rect = Renderer::Rect;
using namespace std::string_literals;

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 720;


static stringstream executeCmd(const string &cmd) {
  auto close = [](FILE *file) { pclose(file); };
  unique_ptr<FILE, decltype(close)> pipe{popen(cmd.c_str(), "r"), close};

  vector<char> buff(0x100);
  size_t n;
  stringstream stream;

  while ((n = fread(buff.data(), sizeof(buff[0]), buff.size(), pipe.get())) >
         0) {
    stream.write(buff.data(), n);
  }
  return stream;
}

static Window createWindow() {
  Window window{"2048"s, 0x1FFF0000, 0x1FFF0000,
                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};
  return window;
}

static Texture createTextureFromSurface(Renderer &renderer, Surface &surface) {
  return Texture{SDL_CreateTextureFromSurface(renderer.get(), surface.get())};
}

static Font createFont(int size) {
  stringstream stream = executeCmd("find /usr/share/fonts -name '*.ttf'");
  string path;
  getline(stream, path);
  if (path.empty()) {
    throw runtime_error{"find no fonts."s};
  }
  return Font{path, size};
}

static vector<Surface> getResImages() {
  vector<string> paths;

  stringstream stream = executeCmd("find ../res -name '*.png'"s);
  string path;
  while (getline(stream, path)) {
    paths.push_back(move(path));
  }

  sort(paths.begin(), paths.end());
  vector<Surface> res;
  for (auto &path : paths) {
    res.push_back(Surface{path});
  }
  return res;
}

void Application::updateLocations(Window::size &size, std::array<std::array<Renderer::Rect, 4>, 4> &locations) {
  int len = size.h > size.w ? size.w : size.h;
  int step = len / 25;
  int rect_len = step * 5;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      locations[i][j].x = (j + 1) * step + j * rect_len;
      locations[i][j].y = (i + 1) * step + i * rect_len;
      locations[i][j].width = rect_len;
      locations[i][j].height = rect_len;
    }
  }
}

static int log(int x) {
  if (x == 0) return 0;

  int res = -1;
  while (x != 0) {
    x = x >> 1;
    res++;
  }
  return res;
};

Application::Application()
    : images_{getResImages()},
      window_{createWindow()},
      renderer_{window_},
      size_{window_.getSize()},
      engine_{device_()} {
  updateLocations(size_, locations_);
}

void Application::run() {
  Font font{createFont(48)};
  Font small{createFont(24)};
  bool quit = false;
  SDL_Event e;
  const uint8_t *keyStates;
  string text = "scores: "s;
  string author = "by ardxwe"s;
  Rect src, dst;
  while (!quit) {
    core(keyState::OTHER);
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_WINDOWEVENT:
          if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
            size_.h = e.window.data2;
            size_.w = e.window.data1;
            std::cout << size_.h << std::endl;
            std::cout << size_.w << std::endl;
            updateLocations(size_, locations_);
          }
          break;
        case SDL_KEYDOWN:
          keyStates = SDL_GetKeyboardState(nullptr);
          switch (state_) {
            case State::STOPPING:
              if (keyStates[SDL_SCANCODE_S]) {
                int first = getRandom();
                int second = getRandom();
                copyTexture(1, first);
                copyTexture(1, second);
                state_ = State::RUNNING;
              }
              break;
            case State::RUNNING: {
              bool end = false;
              for (auto i : map_) {
                if (i == 0) {
                  end = false;
                  break;
                } else {
                  end = true;
                }
              }
              if (end) {
                {
                  Surface over{font, "game over and your scores: "s + std::to_string(scores_), {0xFF, 0xFF, 0xFF}};

                  src = {0, 0, over.getWidth(), over.getHeight()};
                  dst = {(size_.w - over.getWidth()) / 2, (size_.h - over.getHeight()) / 2, over.getWidth(),
                         over.getHeight()};
                  Texture new_t{createTextureFromSurface(renderer_, over)};
                  renderer_.copyTexture(new_t, src, dst);
                }
                state_ = State::STOPPING;
                for (auto &i : map_) {
                  i = 0;
                }
                scores_ = 0;
                renderer_.renderPresent();
                SDL_Delay(1000);
                break;
              }
              if (keyStates[SDL_SCANCODE_UP]) {
                core(keyState::UP);
                copyTexture(1, getRandom());
              } else if (keyStates[SDL_SCANCODE_DOWN]) {
                core(keyState::DOWN);
                copyTexture(1, getRandom());
              } else if (keyStates[SDL_SCANCODE_LEFT]) {
                core(keyState::LEFT);
                copyTexture(1, getRandom());
              } else if (keyStates[SDL_SCANCODE_RIGHT]) {
                core(keyState::RIGHT);
                copyTexture(1, getRandom());
              } else {
                core(keyState::OTHER);
              }
            }
            default:
              break;
          }
      }
    }

    {
      Surface surface{font, text + std::to_string(scores_), {0xFF, 0xFF, 0xFF}};
      src = {0, 0, surface.getWidth(), surface.getHeight()};
      if (size_.h > size_.w) {
        dst = {(size_.w - surface.getWidth()) / 2, size_.w + (size_.h - size_.w - surface.getHeight()) / 2, surface.getWidth(),
               surface.getHeight()};
      } else {
        dst = {size_.h + (size_.w - size_.h - surface.getWidth()) / 2, (size_.h - surface.getHeight()) / 2, surface.getWidth(),
               surface.getHeight()};
      }
      Texture t{createTextureFromSurface(renderer_, surface)};
      renderer_.copyTexture(t, src, dst);
    }
    {
      Surface surface{small, author, {0x0, 0xFF, 0xFF}};
      src = {0, 0, surface.getWidth(), surface.getHeight()};
      dst = {size_.w - surface.getWidth(), size_.h - surface.getHeight(), surface.getWidth(),
             surface.getHeight()};
      Texture t{createTextureFromSurface(renderer_, surface)};
      renderer_.copyTexture(t, src, dst);
    }
    renderer_.renderPresent();
    SDL_Delay(10);
  }
}

int Application::getRandom() {
  int res;
  do {
    res = r_(engine_);
  } while (map_[res]);
  map_[res] = 2;
  return res;
}

void Application::copyTexture(int image_index, int location_index) {
  Texture t{createTextureFromSurface(renderer_, images_[image_index])};
  Renderer::Rect src = {0, 0, images_[image_index].getWidth(), images_[image_index].getHeight()};
  renderer_.copyTexture(t, src, locations_[location_index / 4][location_index % 4]);
}

void Application::core(keyState state) {
  renderer_.clear();
  vector<int> nums(4);
  switch (state) {
    case keyState::UP:
      for (size_t j = 0; j < 4; j++) {
        nums[0] = map_[j];
        nums[1] = map_[4 + j];
        nums[2] = map_[8 + j];
        nums[3] = map_[12 + j];
        nums = merge(nums);

        for (size_t i = 0; i < nums.size(); i++) {
          copyTexture(log(nums[i]), i * 4 + j);
        }
        map_[j] = nums[0];
        map_[4 + j] = nums[1];
        map_[8 + j] = nums[2];
        map_[12 + j] = nums[3];
      }
      break;
    case keyState::DOWN:
      for (size_t j = 0; j < 4; j++) {
        nums[0] = map_[12 + j];
        nums[1] = map_[8 + j];
        nums[2] = map_[4 + j];
        nums[3] = map_[j];
        nums = merge(nums);

        for (size_t i = 0; i < nums.size(); i++) {
          copyTexture(log(nums[i]), (3 - i) * 4 + j);
        }
        map_[12 + j] = nums[0];
        map_[8 + j] = nums[1];
        map_[4 + j] = nums[2];
        map_[j] = nums[3];
      }
      break;
    case keyState::LEFT:
      for (size_t j = 0; j < 4; j++) {
        nums[0] = map_[4 * j];
        nums[1] = map_[1 + 4 * j];
        nums[2] = map_[2 + 4 * j];
        nums[3] = map_[3 + 4 * j];
        nums = merge(nums);

        for (size_t i = 0; i < nums.size(); i++) {
          copyTexture(log(nums[i]), (i + 4 * j));
        }
        map_[4 * j] = nums[0];
        map_[1 + 4 * j] = nums[1];
        map_[2 + 4 * j] = nums[2];
        map_[3 + 4 * j] = nums[3];
      }
      break;
    case keyState::RIGHT:
      for (size_t j = 0; j < 4; j++) {
        nums[0] = map_[3 + 4 * j];
        nums[1] = map_[2 + 4 * j];
        nums[2] = map_[1 + 4 * j];
        nums[3] = map_[4 * j];
        nums = merge(nums);

        for (size_t i = 0; i < nums.size(); i++) {
          copyTexture(log(nums[i]), (3 - i) + 4 * j);
        }
        map_[3 + 4 * j] = nums[0];
        map_[2 + 4 * j] = nums[1];
        map_[1 + 4 * j] = nums[2];
        map_[4 * j] = nums[3];
      }
      break;
    case keyState::OTHER:
      for (size_t i = 0; i < map_.size(); i++) {
        copyTexture(log(map_[i]), i);
      }
  }
}

vector<int> Application::merge(std::vector<int> &nums) {
  vector<int> res;
  size_t count = 0;
  for (size_t i = 0; i < nums.size(); i++) {
    if (nums[i] != 0) {
      count++;
      res.push_back(nums[i]);
    }
  }
  for (size_t i = 0; i < (nums.size() - count); i++) {
    res.push_back(0);
  }
  for (size_t i = 0; i < res.size() - 1; i++) {
    if (res[i] != 0 && res[i] == res[i + 1]) {
      res[i] += res[i];
      scores_ += res[i];
      for (size_t j = i + 1; j < res.size() - 1; j++) {
        res[j] = res[j + 1];
      }
      res[res.size() - 1] = 0;
      break;
    }
  }
  return res;
}