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
#include <SDL_ttf.h>

#include <boost/variant/static_visitor.hpp>

#include <common/Tag.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/geometry/RectangularArea.h>

#include "DrawProp.h"
#include "TextProperties.h"

#include "ClientGui.h"

struct EntityComponentSystem;
struct Drawer;

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

    struct Drawer;

    enum class Color {
        BG,
        INFO_BG,
        MAP_BG,
        TEST,
        TEXT,
        MY_PLAYER,
        OTHER_PLAYER,
        DEFAULT_MAP_OBJECT
    };

    std::map<Color, SDL_Color> mColors = {
            {Color::BG,                 {0,   0,   0,   255}},
            {Color::MAP_BG,             {14,  50,  25,  255}},
            {Color::INFO_BG,            {255, 255, 255, 255}},
            {Color::TEST,               {255, 0,   0,   255}},
            {Color::TEXT,               {0,   0,   0,   255}},
            {Color::OTHER_PLAYER,       {255, 255, 255, 255}},
            {Color::MY_PLAYER,          {255, 0,   0,   255}},
            {Color::DEFAULT_MAP_OBJECT, {127, 127, 127, 255}}
    };

    // Dimensions computed at init
    DrawProp mapBoundingBox { {}, mColors[Color::BG]};
    DrawProp infoBoundingBox { {}, mColors[Color::INFO_BG]};

    DrawProp mapProp { {}, mColors[Color::MAP_BG] };


    TextProperties nameTeamProp = {{}, "", -1};
    TextProperties healthProp = {{}, "", -1};

    std::string mPlayerName, mPlayerTeam;
    EntityID playerId;

    int mFontLoadSize = 200;
    int mFontHeightOffset = 10;

    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;


    friend Drawer;

public:
    ClientGui(const std::string& playerName, const std::string& playerTeam);
    ~ClientGui();

    void renderGui(EntityComponentSystem & entities);

    void drawRect(const DrawProp &dp);


    void initGui();

    void loadMedia();
    void setMapSize(int w, int h);
    void setPlayerHealth(int health);
    void setPlayerId(EntityID id);
    const EntityID getPlayerId() const;

    void render() const;

    void drawAppBg() const;

    void drawClearBg(const SDL_Color &color) const;

    geometry::Point placeToMapCoords(geometry::Point point);

    Scalar scaleToMapCoords(Scalar coord);

    static const geometry::RectangularArea game_area;

    void drawPolygon(geometry::Point center, const std::vector<geometry::Vector> &points,  const SDL_Color& color);

    void drawEntity(const entity_t &entity, const Shape &shape, const Position &position);

    void drawCircle(geometry::Point center, const geometry::CircleShape &shape, const SDL_Color &color);

    void drawText(TextProperties property) const;

    template <typename T>
    T scaleToMapCoords(T coord);

    geometry::Point projectToMapCoords(geometry::Point point);
};
