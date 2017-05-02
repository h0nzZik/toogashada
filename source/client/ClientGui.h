#pragma once

#include <stdexcept>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>

// SDL2
#include <SDL.h>
#include <SDL_video.h>
#include <SDL2_gfxPrimitives.h>

#include <boost/variant/static_visitor.hpp>

#include <common/Tag.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/geometry/RectangularArea.h>

#include "DrawProp.h"

#include "ClientGui.h"

struct EntityComponentSystem;

class ClientGui {
    /* const */ int SCREEN_WIDTH;
    /* const */ int SCREEN_HEIGHT;


    // GUI settings
    DrawProp mapBoundingBoxRatio {
            {0,0,100,90},
            {}
    };

    DrawProp mapRatio {
            {0,0,100,100},
            {}
    };

    enum class Color {
        BG,
        INFO_BG,
        MAP_BG,
        TEST
    };

    std::map<Color, SDL_Color> mColors = {
            {Color::BG,      {0,   0,   0,   255}},
            {Color::MAP_BG,  {14,  50,  25,  255}},
            {Color::INFO_BG, {255, 255, 255, 255}},
            {Color::TEST,    {255, 0,   0,   255}}
    };

    // Dimensions computed at init
    DrawProp mapBoundingBox { {}, mColors[Color::BG]};
    DrawProp infoBoundingBox { {}, mColors[Color::INFO_BG]};

    DrawProp mapProp { {}, mColors[Color::MAP_BG] };


    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;



public:
    ClientGui();
    ~ClientGui();

    void renderGui(EntityComponentSystem & entities);

    //void drawRect();

    void drawRect(const DrawProp &dp);


    void initGui();

    void loadMedia();
    void setMapSize(int w, int h);

    void render() const;

    void drawAppBg() const;

    void drawClearBg(const SDL_Color &color) const;

    geometry::Point projectToMapCoords(geometry::Point point);

    Scalar scaleToMapCoords(Scalar coord);

    geometry::RectangularArea drawing_area() const;

    static const geometry::RectangularArea game_area;

    void drawPolygon(geometry::Point center, const std::vector<geometry::Vector> &points);

    void drawEntity(const entity_t &entity, const Shape &shape, const Position &position);
};
