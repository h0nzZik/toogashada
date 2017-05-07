#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// SDL2
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include <boost/variant/static_visitor.hpp>

#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>
#include <common/geometry/Point.h>
#include <common/geometry/RectangularArea.h>

#include "DrawProp.h"
#include "TextProperties.h"

struct EntityComponentSystem;
struct Drawer;

class ClientController;

class ClientGui {

  ClientController &mController;

  /* const */ int SCREEN_WIDTH;
  /* const */ int SCREEN_HEIGHT;

  // GUI settings
  DrawProp mapBoundingBoxRatio{{0, 0, 100, 90}, {}};

  DrawProp mapRatio{{0, 0, 100, 100}, {}};

  struct EntityDrawer;

  enum class Color {
    BG,
    INFO_BG,
    MAP_BG,
    TEST,
    TEXT,
    MY_PLAYER,
    OTHER_PLAYER,
    DEFAULT_MAP_OBJECT,
    PLAYER_GUN
  };

  std::map<Color, SDL_Color> mColors = {
      {Color::BG, {0, 0, 0, 255}},
      {Color::MAP_BG, {14, 50, 25, 255}},
      {Color::INFO_BG, {255, 255, 255, 255}},
      {Color::TEST, {255, 0, 0, 255}},
      {Color::TEXT, {0, 0, 0, 255}},
      {Color::OTHER_PLAYER, {255, 255, 255, 255}},
      {Color::MY_PLAYER, {255, 255, 255, 255}},
      {Color::DEFAULT_MAP_OBJECT, {127, 127, 127, 255}},
      {Color::PLAYER_GUN, {255, 255, 0, 255}}};

  // Dimensions computed at init
  DrawProp mapBoundingBox{{}, mColors[Color::BG]};
  DrawProp infoBoundingBox{{}, mColors[Color::INFO_BG]};

  DrawProp mapProp{{}, mColors[Color::MAP_BG]};

  TextProperties nameTeamProp = {{}, "", -1};
  TextProperties healthProp = {{}, "", -1};

  std::string mPlayerName, mPlayerTeam;
  std::map<std::string, SDL_Color> teamColors;

  int mFontLoadSize = 200;
  int mFontHeightOffset = 10;

  SDL_Window *mWindow;
  SDL_Renderer *mRenderer;
  TTF_Font *mFont;

  friend Drawer;

  void renderHealth();

public:
  ClientGui(ClientController &controller, const std::string &playerName,
            const std::string &playerTeam, int windowWidth = -1,
            int windowHeight = -1);

  ~ClientGui();

  void renderGui(EntityComponentSystem &entities);

  void drawRect(const DrawProp &dp);

  void initGui();

  void loadMedia();

  void setMapSize(int w, int h);

  void setPlayerHealth(int health);
  void setTeamInfo(const std::vector<TeamInfo> &teamInfo);

  geometry::Point getScreenCoords(const geometry::Point &point) const;

  void render() const;

  void drawAppBg() const;

  void drawClearBg(const SDL_Color &color) const;

  geometry::Point placeToMapCoords(const geometry::Point &point) const;

  Scalar scaleToMapCoords(Scalar coord) const;

  static const geometry::RectangularArea game_area;

  void draw(geometry::Polygon const &polygon, const SDL_Color &color);

  void drawEntity(const entity_t &entity, const Shape &shape,
                  const Position &position);

  void drawCircle(geometry::Point center, Scalar radius,
                  const SDL_Color &color);

  void drawLine(geometry::Point center, Scalar radius, geometry::Angle rotation,
                const SDL_Color &color);

  void drawText(TextProperties property) const;

  template <typename T> T scaleToMapCoords(T coord) const;

  geometry::Point projectToMapCoords(const geometry::Point &point) const;

  void drawPlayer(const Position &pos, const geometry::CircleShape &shape,
                  const PlayerInfo &playerInfo, bool ownPlayer);
};
