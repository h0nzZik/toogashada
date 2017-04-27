//
// Created by lukas on 27/04/17.
//

#ifndef TOOGASHADA_DRAWPROP_H
#define TOOGASHADA_DRAWPROP_H

#include <SDL_rect.h>

struct DrawProp {

    SDL_Rect rect;
    SDL_Color color;

    int& x() { return rect.x; }
    int& y() { return rect.y; }
    int& w() { return rect.w; }
    int& h() { return rect.h; }

    Uint8& r() { return color.r; }
    Uint8& g() { return color.g; }
    Uint8& b() { return color.b; }
    Uint8& a() { return color.a; }
};

#endif //TOOGASHADA_DRAWPROP_H
