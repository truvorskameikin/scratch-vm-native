#include <SDL.h>

#include <cassert>
#include <iostream>

extern "C" {
int SDL_main(int argc, char *argv[]);
}

int SDL_main(int /* argc */, char */* argv */[]) {
  int res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
  if (res < 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
  }
  assert(res == 0);

  auto* pWindow =
      SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       300, 200, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
  assert(pWindow);

  auto* pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

  auto windowID = SDL_GetWindowID(pWindow);

  bool isRunning{true};
  while (isRunning) {
    SDL_Event event;

    while (isRunning && SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT:
          isRunning = false;
          break;
        case SDL_WINDOWEVENT:
          if (event.window.windowID == windowID) {
            switch (event.window.event) {
              case SDL_WINDOWEVENT_MOVED: {
                auto x = event.window.data1;
                auto y = event.window.data2;
                SDL_SetWindowPosition(pWindow, x, y);
                break;
              }

              case SDL_WINDOWEVENT_SIZE_CHANGED: {
                auto width = event.window.data1;
                auto height = event.window.data2;
                SDL_SetWindowSize(pWindow, width, height);
                break;
              }

              case SDL_WINDOWEVENT_CLOSE: {
                event.type = SDL_QUIT;
                SDL_PushEvent(&event);
                break;
              }
            }
          }
      }
    }

    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pRenderer);
    SDL_RenderPresent(pRenderer);
  }

  SDL_DestroyRenderer(pRenderer);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return 0;
}