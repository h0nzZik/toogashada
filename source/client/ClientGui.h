#pragma once

#include <common/GPoint.h>
#include <vector>
#include <SDL.h>
#include <map>
#include <common/GameObjectManager.h>
#include "DrawProp.h"


struct Message;

class ClientGui {

    /* const */ int SCREEN_WIDTH;
    /* const */ int SCREEN_HEIGHT;

    // GUI settings
    DrawProp mapBoundingBoxRatio {
            {0,0,100,90},
            {}
    };


    enum class Color {
        BG,
        INFO_BG,
    };
    std::map<Color, SDL_Color> mColors = {
            {Color::BG, {0, 0, 0, 255}},
            {Color::INFO_BG, {255, 255, 255, 255}},
    };

    // Computed at init
    DrawProp mapBoundingBox { {}, mColors[Color::BG]};
    DrawProp infoBoundingBox { {}, mColors[Color::INFO_BG]};

    void handle_event(SDL_Event const &e);



    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;

    void drawClearBg(const SDL_Color &color) const;

    void drawAppBg() const;

    void render() const;

    void loadMedia();
    void initGui();

public:
    ClientGui();
    ~ClientGui();

    void render_polygon(Point center, std::vector<Point> const &points);

    void renderGui(GameObjectManager &gameObjects);

    void drawRect();

    void drawRect(DrawProp &dp);
};