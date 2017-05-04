//
// Created by lukas on 27/04/17.
//

#ifndef TOOGASHADA_DRAWPROP_H
#define TOOGASHADA_DRAWPROP_H

#include <SDL_rect.h>

struct DrawProp {

  SDL_Rect rect;
  SDL_Color color;

  int &x() { return rect.x; }
  int &y() { return rect.y; }
  int &w() { return rect.w; }
  int &h() { return rect.h; }

  const int &x() const { return rect.x; }
  const int &y() const { return rect.y; }
  const int &w() const { return rect.w; }
  const int &h() const { return rect.h; }

  Uint8 &r() { return color.r; }
  Uint8 &g() { return color.g; }
  Uint8 &b() { return color.b; }
  Uint8 &a() { return color.a; }

  const Uint8 &r() const { return color.r; }
  const Uint8 &g() const { return color.g; }
  const Uint8 &b() const { return color.b; }
  const Uint8 &a() const { return color.a; }
};

#endif // TOOGASHADA_DRAWPROP_H
