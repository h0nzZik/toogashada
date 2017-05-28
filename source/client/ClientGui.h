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

// Boost
#include <boost/variant/static_visitor.hpp>

// Common
#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>
#include <common/geometry/Point.h>
#include <common/geometry/RectangularArea.h>

// Client
#include "DrawProp.h"
#include "TextProperties.h"
#include "SDLDeleter.h"

struct EntityComponentSystem;
struct EntityDrawer;

class ClientController;

class ClientGui {

	struct EntityDrawer;
	friend EntityDrawer;

  ClientController &mController;
	std::string mPlayerName, mPlayerTeam;
	std::map<std::string, SDL_Color> teamColors;

	MySDL::SDL sdl{SDL_INIT_VIDEO};
	MySDL::TTF ttf;
	MySDL::Window mWindow;
	//SDL_Window *mWindow;
	MySDL::Renderer mRenderer;
	//SDL_Renderer *mRenderer;
	MySDL::Font mFont;
	//TTF_Font *mFont;

  /* const */ int SCREEN_WIDTH;
  /* const */ int SCREEN_HEIGHT;

  // GUI settings
  DrawProp mapBoundingBoxRatio{{0, 0, 100, 90}, {}};
  DrawProp mapRatio{{0, 0, 100, 100}, {}};

  enum class Color {
    BG,
    INFO_BG,
    MAP_BG,
    TEXT,
    MY_PLAYER,
    DEFAULT_MAP_OBJECT,
    PLAYER_GUN
  };

  std::map<Color, SDL_Color> mColors = {
				  {Color::BG,                 {0,   0,   0,   255}},
				  {Color::MAP_BG,             {200,  200,  200,  255}},//{14,  50,  25,  255}},
				  {Color::INFO_BG,            {255, 255, 255, 255}},
				  {Color::TEXT,               {0,   0,   0,   255}},
				  {Color::MY_PLAYER,          {255, 255, 255, 255}},
				  {Color::DEFAULT_MAP_OBJECT, {0,   0,   0,   255}},
				  {Color::PLAYER_GUN,         {255, 255, 0,   255}}};

	int mFontLoadSize = 200;
	int mFontHeightOffset = 10;

  // Dimensions computed at init
  DrawProp mapBoundingBox{{}, mColors[Color::BG]};
  DrawProp infoBoundingBox{{}, mColors[Color::INFO_BG]};
  DrawProp mapProp{{}, mColors[Color::MAP_BG]};
  TextProperties nameTeamProp = {{}, "", -1};
  TextProperties healthProp = {{}, "", -1};



	void initGui();
	void loadMedia();

	void drawAppBg() const;
	void drawClearBg(const SDL_Color &color) const;

	void drawRect(const DrawProp &dp);
	void drawCircle(geometry::Point center, Scalar radius, const SDL_Color &color);
	void drawLine(geometry::Point center, Scalar radius, geometry::Angle rotation, const SDL_Color &color);
	void drawText(TextProperties property) const;

	void draw(geometry::Polygon const &polygon, const SDL_Color &color);
	void drawEntity(const entity_t &entity, const Shape &shape, const Position &position);
	void drawPlayer(const Position &pos, const geometry::CircleShape &shape, const PlayerInfo &playerInfo, bool ownPlayer);
	void drawHealth();

	void render() const;


	geometry::Point placeToMapCoords(const geometry::Point &point) const;
	Scalar scaleToMapCoords(Scalar coord) const;
	template <typename T> T scaleToMapCoords(T coord) const;
	geometry::Point projectToMapCoords(const geometry::Point &point) const;

public:
  ClientGui(ClientController &controller, const std::string &playerName,
            const std::string &playerTeam, int windowWidth = -1,
            int windowHeight = -1);

  ~ClientGui();

	void setMapSize(int w, int h);
	void setTeamInfo(const std::vector<TeamInfo> &teamInfo);
	void renderGui(EntityComponentSystem &entities);
	geometry::Point getScreenCoords(const geometry::Point &point) const;

};
