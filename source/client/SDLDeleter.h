//
// Created by lukas on 4/25/17.
//

#ifndef TOOGASHADA_SDLDELETER_H
#define TOOGASHADA_SDLDELETER_H

#include <SDL_system.h>
#include <SDL_video.h>
#include <memory>
#include <stdexcept>
#include <string>

extern "C" void TTF_CloseFont(TTF_Font *font);
extern "C" void SDL_DestroyWindow(SDL_Window *window);

extern "C" int SDL_Init(Uint32 flags);
extern "C" void SDL_Quit();
extern "C" int TTF_Init();
extern "C" void TTF_Quit();

namespace MySDL {
struct SDLDeleter {
  void operator()(SDL_Window *ptr) const { SDL_DestroyWindow(ptr); }
  void operator()(SDL_Renderer *ptr) const { SDL_DestroyRenderer(ptr); }
  void operator()(SDL_Texture *ptr) const { SDL_DestroyTexture(ptr); }
  void operator()(TTF_Font *ptr) const { TTF_CloseFont(ptr); }
};

using Window = std::unique_ptr<SDL_Window, SDLDeleter>;
using Renderer = std::unique_ptr<SDL_Renderer, SDLDeleter>;
using Font = std::unique_ptr<TTF_Font, SDLDeleter>;

class SDL {
public:
  SDL(uint32_t flags) {
    if (SDL_Init(flags) < 0)
      throw std::runtime_error("Cannot initialize SDL: " +
                               std::string(SDL_GetError()));
  }

  ~SDL() { SDL_Quit(); }
};

class TTF {
public:
  TTF() {
    if (TTF_Init() < 0)
      throw std::runtime_error("Cannot initialize TTF");
  }

  ~TTF() { TTF_Quit(); }
};

} // namespace MySDL

#endif // TOOGASHADA_SDLDELETER_H
