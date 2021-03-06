// Standard
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// SDL2
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

// Boost
#include <boost/variant/static_visitor.hpp>

// ColorConverter
#include <ColorConverter.h>

// Common
#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>
#include <common/geometry/Point.h>
#include <common/geometry/RectangularArea.h>

// Client
#include "ClientController.h"
#include "DrawProp.h"
#include "SDLDeleter.h"
#include "SDLWrapper.h"
#include "TextProperties.h"

// Self
#include "ClientGui.h"

using namespace std;
using namespace geometry;

class ClientGui::Impl {

  struct EntityDrawer;
  friend EntityDrawer;

  ClientController &mController;
  std::string mPlayerName, mPlayerTeam;
  std::map<std::string, SDL_Color> teamColors;

  SDLw::SDL sdl{SDL_INIT_VIDEO};
  SDLw::TTF ttf;
  MySDL::Window mWindow;
  // SDL_Window *mWindow;
  MySDL::Renderer mRenderer;
  // SDL_Renderer *mRenderer;
  MySDL::Font mFont;
  // TTF_Font *mFont;

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
      {Color::BG, {0, 0, 0, 255}},
      {Color::MAP_BG, {200, 200, 200, 255}}, //{14,  50,  25,  255}},
      {Color::INFO_BG, {255, 255, 255, 255}},
      {Color::TEXT, {0, 0, 0, 255}},
      {Color::MY_PLAYER, {255, 255, 255, 255}},
      {Color::DEFAULT_MAP_OBJECT, {0, 0, 0, 255}},
      {Color::PLAYER_GUN, {255, 255, 0, 255}}};

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
  void drawCircle(geometry::Point center, Scalar radius,
                  const SDL_Color &color);
  void drawLine(geometry::Point center, Scalar radius, geometry::Angle rotation,
                const SDL_Color &color);
  void drawText(TextProperties property) const;

  void draw(geometry::Polygon const &polygon, const SDL_Color &color);
  void drawEntity(const entity_t &entity, const Shape &shape,
                  const Position &position);
  void drawPlayer(const Position &pos, const geometry::CircleShape &shape,
                  const PlayerInfo &playerInfo, bool ownPlayer);
  void drawHealth();

  void render() const;

  struct WH {
    int width;
    int height;
  };

  WH textBoxDimensions(TextProperties const &property,
                       SDL_Surface *textSurface) const {
    int fontSize = property.mSize;
    DrawProp const &dp = property.mDrawProp;

    if (fontSize < 0) {

      double ratioSource = static_cast<double>(textSurface->w) / textSurface->h;
      double ratioTarget = static_cast<double>(dp.w()) / dp.h();

      if (ratioSource > ratioTarget) {
        return {dp.w(), dp.w() / ratioSource};
      }
      return { dp.h() * ratioSource, dp.h() };
    }

    double multiplier = static_cast<double>(fontSize) / mFontLoadSize;
    return { textSurface->w *multiplier, textSurface->h *multiplier };
  }

  geometry::Point placeToMapCoords(const geometry::Point &point) const;
  Scalar scaleToMapCoords(Scalar coord) const;
  template <typename T> T scaleToMapCoords(T coord) const;
  geometry::Point projectToMapCoords(const geometry::Point &point) const;

public:
  Impl(ClientController &controller, const std::string &playerName,
       const std::string &playerTeam, int windowWidth = -1,
       int windowHeight = -1);

  ~Impl();

  void setMapSize(int w, int h);
  void setTeamInfo(const std::vector<TeamInfo> &teamInfo);
  void renderGui(EntityComponentSystem &entities);
  geometry::Point getScreenCoords(const geometry::Point &point) const;
};

struct ClientGui::Impl::EntityDrawer : public boost::static_visitor<void> {
  ClientGui::Impl &gui;
  entity_t const &entity;
  Position const &position;
  SDL_Color color = gui.mColors[Color::DEFAULT_MAP_OBJECT];

  EntityDrawer(ClientGui::Impl &gui, entity_t const &entity,
               Position const &position)
      : gui(gui), entity(entity), position(position) {}

  void operator()(PolygonalShape const &shape) {
    gui.draw(geometry::createPolygon(position.center, position.rotation, shape),
             color);
  }

  void operator()(CircleShape const &shape) {
    if (entity.has_component<PlayerInfo>()) {
      gui.drawPlayer(
          position, shape, entity.get_component<PlayerInfo>(),
          gui.mController.isMyPlayer(entity.get_component<EntityID>()));
    } else {
      SDL_Color color = {255, 0, 0, 255};
      gui.drawCircle(gui.projectToMapCoords(position.center),
                     gui.scaleToMapCoords(shape.radius), color);
    }
  }
};

ClientGui::Impl::Impl(ClientController &controller,
                      const std::string &playerName,
                      const std::string &playerTeam, int windowWidth,
                      int windowHeight)
    : mController{controller}, mPlayerName{playerName}, mPlayerTeam{
                                                            playerTeam} {

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  // Screen dimension constants
  SCREEN_WIDTH = windowWidth < 0 || windowWidth > dm.w
                     ? static_cast<int>(dm.w * 0.9)
                     : windowWidth;
  SCREEN_HEIGHT = windowHeight < 0 || windowHeight > dm.h
                      ? static_cast<int>(dm.h * 0.9)
                      : windowHeight;

  cout << "Width: " << SCREEN_WIDTH << " Height: " << SCREEN_HEIGHT << endl;

  mWindow = MySDL::Window(SDL_CreateWindow(
      "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN));
  if (!mWindow)
    throw std::runtime_error("Cannot create window: " + string(SDL_GetError()));

  mRenderer = MySDL::Renderer(
      SDL_CreateRenderer(mWindow.get(), -1, SDL_RENDERER_ACCELERATED));
  if (!mRenderer)
    throw std::runtime_error("Cannot create renderer: " +
                             string(SDL_GetError()));

  loadMedia();
  render();

  initGui();
}

ClientGui::Impl::~Impl() { ; }

void ClientGui::Impl::initGui() {
  {
    auto bbr = mapBoundingBoxRatio;
    auto &bb = mapBoundingBox;

    bb.w() = (bbr.w() * SCREEN_WIDTH) / 100;
    bb.h() = (bbr.h() * SCREEN_HEIGHT) / 100;
    bb.x() = (bbr.x() * SCREEN_WIDTH) / 100;
    bb.y() = (bbr.y() * SCREEN_HEIGHT) / 100;
  }

  {
    auto &mb = mapBoundingBox;
    auto &bb = infoBoundingBox;

    bb.w() = mb.w();
    bb.h() = SCREEN_HEIGHT - mb.h();
    bb.x() = mb.y();
    bb.y() = mb.h();
  }

  {
    auto &mbb = mapBoundingBox;
    auto &mr = mapRatio;
    auto &mp = mapProp;

    double mapRatio = mr.w() / static_cast<double>(mr.h());
    double mpBoundingBoxRatio = mbb.w() / static_cast<double>(mbb.h());

    if (mapRatio > mpBoundingBoxRatio) {

      mp.w() = mbb.w();
      mp.h() = static_cast<int>(mp.w() / mapRatio);
      mp.x() = mbb.x();
      mp.y() = mbb.y() + ((mbb.h() - mp.h()) / 2);

    } else {

      mp.h() = mbb.h();
      mp.w() = static_cast<int>(mp.h() * mapRatio);
      mp.x() = mbb.x() + ((mbb.w() - mp.w()) / 2);
      mp.y() = mbb.y();
    }
  }

  {

    int padding = 10;

    auto &np = nameTeamProp.mDrawProp;
    auto &ib = infoBoundingBox;

    np.x() = ib.x() + padding;
    np.y() = ib.y() + padding;
    np.h() = ib.h() - 2 * padding;
    np.w() = (ib.w() / 3) - 2 * padding;

    np.color = mColors[Color::TEXT];

    nameTeamProp.mText = "[" + mPlayerTeam + "] " + mPlayerName;
    nameTeamProp.mSize = -1;
  }

  {
    int padding = 10;

    auto &hp = healthProp.mDrawProp;
    auto &np = nameTeamProp.mDrawProp;
    auto &ib = infoBoundingBox;

    hp.x() = np.x() + np.w() + padding;
    hp.y() = ib.y() + padding;
    hp.h() = ib.h() - 2 * padding;
    hp.w() = (ib.w() / 3) - 2 * padding;

    hp.color = mColors[Color::TEXT];

    healthProp.mText = "HP: -";
    nameTeamProp.mSize = -1;
  }
}

void ClientGui::Impl::loadMedia() {
  mFont = MySDL::Font(TTF_OpenFont("Biotypc.ttf", mFontLoadSize));
  if (!mFont)
    throw std::runtime_error("Cannot load font.");
}

Scalar ClientGui::Impl::scaleToMapCoords(Scalar coord) const {

  return coord * (static_cast<float>(mapProp.w()) / mapRatio.w());
}

template <typename T> T ClientGui::Impl::scaleToMapCoords(T coord) const {

  static_assert(std::is_same<T, Vector>::value || std::is_same<T, Point>::value,
                "Cannot scale given type.");

  return T{coord.x * (static_cast<float>(mapProp.w()) / mapRatio.w()),
           coord.y * (static_cast<float>(mapProp.h()) / mapRatio.h())};
}

geometry::Point
ClientGui::Impl::placeToMapCoords(const geometry::Point &point) const {

  return {mapProp.x() + point.x, mapProp.y() + point.y};
}

geometry::Point
ClientGui::Impl::projectToMapCoords(const geometry::Point &point) const {

  return placeToMapCoords(scaleToMapCoords(point));
}

geometry::Point
ClientGui::Impl::getScreenCoords(const geometry::Point &point) const {

  return projectToMapCoords(point);
}

void ClientGui::Impl::drawClearBg(const SDL_Color &color) const {

  SDL_SetRenderDrawColor(mRenderer.get(), color.r, color.g, color.b, color.a);
  SDL_RenderClear(mRenderer.get());
}

void ClientGui::Impl::drawAppBg() const { drawClearBg(mColors.at(Color::BG)); }

void ClientGui::Impl::render() const { SDL_RenderPresent(mRenderer.get()); }

void ClientGui::Impl::drawText(TextProperties property) const {

  std::string &text = property.mText;
  DrawProp &dp = property.mDrawProp;

  SDL_Surface *textSurface =
      TTF_RenderText_Blended(mFont.get(), text.c_str(), dp.color);

  if (textSurface == nullptr) {
    cerr << "Error creating text. Err: " << TTF_GetError();
    return;
  }

  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(mRenderer.get(), textSurface);
  if (texture == nullptr) {
    cerr << "Error creating texture text. Err: " << SDL_GetError();
    return;
  }

  WH dim = textBoxDimensions(property, textSurface);

  SDL_FreeSurface(textSurface);

  int correction = static_cast<int>(
      dim.height * (static_cast<double>(mFontHeightOffset) / 100));
  int centeredX = dp.x() + (dp.w() / 2 - dim.width / 2);
  int centeredY = (dp.y() + (dp.h() / 2 - dim.height / 2)) + correction;

  SDL_Rect boundingBox = {centeredX, centeredY, dim.width, dim.height};
  SDL_SetTextureAlphaMod(texture, dp.color.a);

  SDL_RenderCopy(mRenderer.get(), texture, nullptr, &boundingBox);

  SDL_DestroyTexture(texture);
}

void ClientGui::Impl::draw(geometry::Polygon const &polygon,
                           const SDL_Color &color) {
  size_t const n = polygon.size();
  auto xs = make_unique<Sint16[]>(n);
  auto ys = make_unique<Sint16[]>(n);

  for (size_t i = 0; i < n; i++) {
    geometry::Point point = projectToMapCoords(polygon[i]);
    xs[i] = point.x;
    ys[i] = point.y;
  }

  filledPolygonRGBA(mRenderer.get(), xs.get(), ys.get(), n, color.r, color.g,
                    color.b, color.a);
}

void ClientGui::Impl::drawCircle(geometry::Point center, Scalar radius,
                                 const SDL_Color &color) {

  filledCircleRGBA(mRenderer.get(), center.x, center.y, radius, color.r,
                   color.g, color.b, color.a);
}

void ClientGui::Impl::drawHealth() {

  entity_t my_player = mController.getMyPlayer();

  if (my_player.get_status() != entityplus::entity_status::UNINITIALIZED &&
      my_player.sync()) {
    if (my_player.has_component<Health>()) {
      healthProp.mText =
          "HP: " + to_string(my_player.get_component<Health>().hp);
    }
  }
  drawText(healthProp);
}

void ClientGui::Impl::renderGui(EntityComponentSystem &entities) {
  using namespace std::placeholders;

  drawAppBg();
  drawRect(mapBoundingBox);
  drawRect(infoBoundingBox);
  drawRect(mapProp);
  drawText(nameTeamProp);
  drawHealth();

  entities.entityManager.for_each<Shape, Position>(
      std::bind(&ClientGui::Impl::drawEntity, this, _1, _2, _3));

  render();
}

void ClientGui::Impl::drawPlayer(const Position &position,
                                 const CircleShape &shape,
                                 const PlayerInfo &playerInfo, bool ownPlayer) {

  Point projectedCenter = projectToMapCoords(position.center);
  Scalar scaledRadius = scaleToMapCoords(shape.radius);

  int mouseX;
  int mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);

  drawCircle(projectedCenter, scaledRadius, teamColors[playerInfo.mTeam]);
  if (ownPlayer) {
    drawCircle(projectedCenter, scaledRadius / 2, mColors[Color::MY_PLAYER]);
  }
  drawLine(projectedCenter, scaledRadius, position.rotation,
           mColors[Color::PLAYER_GUN]);

  float padding = 0.8;
  int x = projectedCenter.x;
  int y = projectedCenter.y;
  int radiusPadded = scaledRadius * padding;
  drawText({{{x - radiusPadded, y - radiusPadded, radiusPadded * 2,
              radiusPadded * 2},
             mColors[Color::TEXT]},
            playerInfo.mName,
            -1});
}

void ClientGui::Impl::drawEntity(entity_t const &entity, Shape const &shape,
                                 Position const &position) {

  EntityDrawer processor{*this, entity, position};
  boost::apply_visitor(processor, shape);
}

void ClientGui::Impl::drawRect(const DrawProp &dp) {

  SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(mRenderer.get(), dp.color.r, dp.color.g, dp.color.b,
                         dp.color.a);
  SDL_Rect rect = {dp.x(), dp.y(), dp.w(), dp.h()};
  SDL_RenderFillRect(mRenderer.get(), &rect);
}

void ClientGui::Impl::setMapSize(int w, int h) {

  mapRatio.w() = w;
  mapRatio.h() = h;

  initGui();
}

void ClientGui::Impl::setTeamInfo(const std::vector<TeamInfo> &teamInfo) {

  int teamCount = teamInfo.size();

  for (auto &team : teamInfo) {

    int teamId = team.mId;
    float factor = static_cast<float>(teamId) / teamCount;
    teamColors[team.mName] = HSVtoRGB(static_cast<int>(360 * factor), 1, 1);
  }
}

void ClientGui::Impl::drawLine(geometry::Point center, Scalar radius,
                               geometry::Angle rotation,
                               const SDL_Color &color) {

  int thickness = 2;
  int radiusInt = static_cast<int>(radius);

  SDL_Rect target = {static_cast<int>(center.x) - thickness / 2,
                     static_cast<int>(center.y) - radiusInt, thickness,
                     radiusInt};

  SDL_Texture *texture = SDL_CreateTexture(
      mRenderer.get(), SDL_GetWindowPixelFormat(mWindow.get()),
      SDL_TEXTUREACCESS_TARGET, thickness, radius);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(mRenderer.get(), texture);
  SDL_SetRenderDrawColor(mRenderer.get(), 255, 255, 255, 0);
  SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
  SDL_RenderClear(mRenderer.get());
  SDL_SetRenderDrawColor(mRenderer.get(), color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(mRenderer.get(), nullptr);
  SDL_SetRenderTarget(mRenderer.get(), nullptr);

  SDL_Point ref = {target.w / 2, radiusInt};
  SDL_RenderCopyEx(mRenderer.get(), texture, NULL, &target, rotation, &ref,
                   SDL_RendererFlip::SDL_FLIP_NONE);
}

ClientGui::ClientGui(ClientController &controller,
                     const std::string &playerName,
                     const std::string &playerTeam, int windowWidth,
                     int windowHeight) {
  impl = make_unique<Impl>(controller, playerName, playerTeam, windowWidth,
                           windowHeight);
}

ClientGui::~ClientGui() = default;

void ClientGui::setMapSize(int w, int h) { return impl->setMapSize(w, h); }

void ClientGui::setTeamInfo(const std::vector<TeamInfo> &teamInfo) {
  return impl->setTeamInfo(teamInfo);
}

void ClientGui::renderGui(EntityComponentSystem &entities) {
  return impl->renderGui(entities);
}

geometry::Point ClientGui::getScreenCoords(const geometry::Point &point) const {
  return impl->getScreenCoords(point);
}
