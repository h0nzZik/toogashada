//
// Created by lukas on 4/25/17.
//

#ifndef TOOGASHADA_SDLDELETER_H
#define TOOGASHADA_SDLDELETER_H

#include <SDL_system.h>
#include <SDL_video.h>

struct SDLDeleter {
  void operator()(SDL_Window *ptr) const { SDL_DestroyWindow(ptr); }
  void operator()(SDL_Renderer *ptr) const { SDL_DestroyRenderer(ptr); }
  void operator()(SDL_Texture *ptr) const { SDL_DestroyTexture(ptr); }
};

#endif // TOOGASHADA_SDLDELETER_H
