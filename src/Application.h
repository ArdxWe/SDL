//
// Created by ardxwe on 2021/2/6.
//

#ifndef DEMO_APPLICATION_H
#define DEMO_APPLICATION_H

#include "App.h"
#include "Image.h"
#include "Renderer.h"
#include "Surface.h"
#include "Texture.h"
#include "Ttf.h"
#include "Window.h"
#include <SDL2/SDL.h>
#include <future>
#include <random>
#include <string>
#include <vector>

class Application {
  public:
  Application();
  void run();
  int getRandom();
  void copyTexture(int image_index, int location_index);
  enum class keyState {
    UP,
    DOWN,
    LEFT,
    RIGHT,
  };
  void core(keyState state);
  std::vector<int> merge(std::vector<int> &nums);
  void init();

  private:
  App app_{};
  Image image_app_{};
  Ttf ttf_app_{};
  std::vector<Surface> images_;
  Window window_;
  Renderer renderer_;
  Window::size size_;
  std::random_device device_{};
  std::default_random_engine engine_;
  std::uniform_int_distribution<int> r_{0, 4 * 4 - 1};

  std::array<std::array<Renderer::Rect, 4>, 4> locations_;
  std::array<int, 16> map_{};

  enum class State {
    RUNNING,
    STOPPING,
  };
  State state_ = State::STOPPING;
  uint32_t scores_ = 0;
};

#endif// DEMO_APPLICATION_H
