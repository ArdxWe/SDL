#include "App.h"
#include "Surface.h"
#include "Window.h"
#include <SDL2/SDL.h>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[]) {
  App app{};
  Window window{"SDL Tutorial", 0x1FFF0000,    0x1FFF0000,
                SCREEN_WIDTH,   SCREEN_HEIGHT, SDL_WINDOW_SHOWN};
  Surface surface{window};

  // Fill the surface white
  surface.fill_rect(nullptr, SDL_MapRGB(surface.get_format(), 0x0, 0x0, 0x0));

  // Update the surface
  SDL_UpdateWindowSurface(window.get());

  // Wait two seconds
  SDL_Delay(9000);

  return 0;
}
