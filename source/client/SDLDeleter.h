//
// Created by lukas on 4/25/17.
//

#ifndef TOOGASHADA_SDLDELETER_H
#define TOOGASHADA_SDLDELETER_H

#include <memory>
#include <SDL_system.h>
#include <SDL_video.h>

extern "C" void TTF_CloseFont(TTF_Font *font);
extern "C" void SDL_DestroyWindow(SDL_Window* window);

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

} // namespace MySDL

#endif // TOOGASHADA_SDLDELETER_H
