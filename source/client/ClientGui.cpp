#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

// SDL2
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_video.h>

#include <boost/variant/static_visitor.hpp>

#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>
#include <common/geometry/RectangularArea.h>

#include "ClientController.h"
#include "DrawProp.h"

#include "ClientGui.h"
#include "ColorConverter.h"

using namespace std;
using namespace geometry;

// TODO standalone class for game area

ClientGui::ClientGui(ClientController &controller,
                     const std::string &playerName, const std::string &playerTeam,
                     int windowWidth, int windowHeight)
        : mController{controller}, mPlayerName{playerName},
          mPlayerTeam{playerTeam} {

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0)
        throw std::runtime_error("Cannot initialize SDL: " +
                                 string(SDL_GetError()));

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    // Screen dimension constants
    SCREEN_WIDTH =  windowWidth < 0 || windowWidth > dm.w ? static_cast<int>(dm.w * 0.9) : windowWidth;
    SCREEN_HEIGHT = windowHeight < 0 || windowHeight > dm.h ? static_cast<int>(dm.h * 0.9) : windowHeight;

    cout << "Width: " << SCREEN_WIDTH << " Height: " << SCREEN_HEIGHT << endl;

    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!mWindow)
        throw std::runtime_error("Cannot create window: " + string(SDL_GetError()));

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer)
        throw std::runtime_error("Cannot create renderer: " +
                                 string(SDL_GetError()));

    loadMedia();
    render();

    initGui();
}

ClientGui::~ClientGui() {

    TTF_CloseFont(mFont);
    mFont = nullptr;

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    mRenderer = nullptr;
    mWindow = nullptr;

    TTF_Quit();
    SDL_Quit();
}

Scalar ClientGui::scaleToMapCoords(Scalar coord) const {

    return coord * (static_cast<float>(mapProp.w()) / mapRatio.w());
}

template<typename T>
T ClientGui::scaleToMapCoords(T coord) const {

    static_assert(std::is_same<T, Vector>::value || std::is_same<T, Point>::value,
                  "Cannot scale given type.");

    return T{coord.x * (static_cast<float>(mapProp.w()) / mapRatio.w()),
             coord.y * (static_cast<float>(mapProp.h()) / mapRatio.h())};
}

geometry::Point
ClientGui::placeToMapCoords(const geometry::Point &point) const {

    return {mapProp.x() + point.x, mapProp.y() + point.y};
}

geometry::Point
ClientGui::projectToMapCoords(const geometry::Point &point) const {

    return placeToMapCoords(scaleToMapCoords(point));
}

void ClientGui::drawClearBg(const SDL_Color &color) const {

    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(mRenderer);
}

void ClientGui::drawAppBg() const { drawClearBg(mColors.at(Color::BG)); }

void ClientGui::render() const { SDL_RenderPresent(mRenderer); }

void ClientGui::loadMedia() {

    mFont = TTF_OpenFont("Biotypc.ttf", mFontLoadSize);

    if (mFont == nullptr) {

        std::cerr << "Font cannot load.";
    }
}

void ClientGui::drawText(TextProperties property) const {

    int fontSize = property.mSize;
    std::string &text = property.mText;
    DrawProp &dp = property.mDrawProp;

    SDL_Surface *textSurface =
            TTF_RenderText_Blended(mFont, text.c_str(), dp.color);

    if (textSurface == nullptr) {

        cerr << "Error creating text. Err: " << TTF_GetError();
    } else {

        SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        if (texture == nullptr) {

            cerr << "Error creating texture text. Err: " << SDL_GetError();
        } else {

            int textBoxW, textBoxH;

            if (fontSize < 0) {

                double ratioSource =
                        static_cast<double>(textSurface->w) / textSurface->h;
                double ratioTarget = static_cast<double>(dp.w()) / dp.h();

                if (ratioSource > ratioTarget) {

                    textBoxW = dp.w();
                    textBoxH = textBoxW / ratioSource;

                } else {

                    textBoxH = dp.h();
                    textBoxW = textBoxH * ratioSource;
                }
            } else {

                double multiplier = static_cast<double>(fontSize) / mFontLoadSize;
                textBoxW = textSurface->w * multiplier;
                textBoxH = textSurface->h * multiplier;
            }

            SDL_FreeSurface(textSurface);

            int correction = static_cast<int>(
                    textBoxH * (static_cast<double>(mFontHeightOffset) / 100));
            int centeredX = dp.x() + (dp.w() / 2 - textBoxW / 2);
            int centeredY = (dp.y() + (dp.h() / 2 - textBoxH / 2)) + correction;

            SDL_Rect boundingBox = {centeredX, centeredY, textBoxW, textBoxH};
            SDL_SetTextureAlphaMod(texture, dp.color.a);

            SDL_RenderCopy(mRenderer, texture, nullptr, &boundingBox);

            SDL_DestroyTexture(texture);
        }
    }
}

void ClientGui::initGui() {
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

void ClientGui::draw(geometry::Polygon const &polygon, const SDL_Color &color) {
    size_t const n = polygon.size();
    auto xs = make_unique<Sint16[]>(n);
    auto ys = make_unique<Sint16[]>(n);

    for (size_t i = 0; i < n; i++) {
        geometry::Point point = projectToMapCoords(polygon[i]);
        xs[i] = point.x;
        ys[i] = point.y;
    }

    polygonRGBA(mRenderer, xs.get(), ys.get(), n, color.r, color.g, color.b,
                color.a);
}

void ClientGui::drawCircle(geometry::Point center, Scalar radius, const SDL_Color &color) {

    filledCircleRGBA(mRenderer, center.x, center.y,
                     radius, color.r, color.g, color.b,
                     color.a);
}

void ClientGui::renderHealth() {
	entity_t my_player = mController.getMyPlayer();
	//entity_t my_player;
	if(my_player.get_status() != entityplus::entity_status::UNINITIALIZED && my_player.sync()) {
		if (my_player.has_component<Health>()) {
		    healthProp.mText = "HP: " + to_string(my_player.get_component<Health>().hp);
		}
	}
    drawText(healthProp);
}

void ClientGui::renderGui(EntityComponentSystem &entities) {
    using namespace std::placeholders;

    drawAppBg();
    drawRect(mapBoundingBox);
    drawRect(infoBoundingBox);
    drawRect(mapProp);
    drawText(nameTeamProp);
	renderHealth();

    entities.entityManager.for_each<Shape, Position>(
            std::bind(&ClientGui::drawEntity, this, _1, _2, _3));

    render();
}

struct ClientGui::EntityProcessor : public boost::static_visitor<void> {
    ClientGui &gui;
    entity_t const &entity;
    Position const &position;
    SDL_Color color = gui.mColors[Color::DEFAULT_MAP_OBJECT];
    bool myPlayer = false;

    EntityProcessor(ClientGui &gui, entity_t const &entity, Position const &position)
            : gui(gui), entity(entity), position(position) {}

    void operator()(PolygonalShape const &shape) {

        cout << position.rotation << endl;
        gui.draw(geometry::createPolygon(position.center, position.rotation, shape),
                 color);
    }

    void operator()(CircleShape const &shape) {

        if (entity.has_component<PlayerInfo>()) {

            // TODO > I have a dilemma with where to put the health update, this will happen much more often than necessary,
            // TODO > but having it update via separate server message is duplicity with PlayerInfo component
            gui.setPlayerHealth(entity.get_component<PlayerInfo>().hp); // TODO remove
            gui.drawPlayer(position, shape, entity.get_component<PlayerInfo>(), gui.mController.isMyPlayer(entity.get_component<EntityID>()));

        } else {

            gui.drawCircle(gui.projectToMapCoords(position.center),
                           gui.scaleToMapCoords(shape.radius),
                           color);
        }
    }
};

void ClientGui::drawPlayer(const Position &position, const CircleShape &shape, const PlayerInfo& playerInfo, bool ownPlayer) {


    Point projectedCenter = projectToMapCoords(position.center);
    Scalar scaledRadius = scaleToMapCoords(shape.radius);

    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    drawCircle(projectedCenter, scaledRadius, teamColors[playerInfo.mTeam]);
    if (ownPlayer) {
        drawCircle(projectedCenter, scaledRadius / 2, mColors[Color::MY_PLAYER]);
    }
    drawLine(projectedCenter, scaledRadius, position.rotation, mColors[Color::PLAYER_GUN]);

    float padding = 0.8;
    int x = projectedCenter.x;
    int y = projectedCenter.y;
    int radiusPadded = scaledRadius * padding;
    drawText({
                     {{x - radiusPadded, y - radiusPadded, radiusPadded * 2, radiusPadded * 2},
                      mColors[Color::TEXT]},
                     playerInfo.mName,
                     -1
             });
}

void ClientGui::drawEntity(entity_t const &entity, Shape const &shape,
                           Position const &position) {

    EntityProcessor processor{*this, entity, position};

    boost::apply_visitor(processor, shape);
}

void ClientGui::drawRect(const DrawProp &dp) {

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, dp.color.r, dp.color.g, dp.color.b,
                           dp.color.a);
    SDL_Rect rect = {dp.x(), dp.y(), dp.w(), dp.h()};
    SDL_RenderFillRect(mRenderer, &rect);
}

void ClientGui::setMapSize(int w, int h) {

    mapRatio.w() = w;
    mapRatio.h() = h;

    initGui();
}

void ClientGui::setTeamInfo(const std::vector<TeamInfo>& teamInfo) {

    int teamCount = teamInfo.size();

    for (auto &team : teamInfo) {
        int teamId = team.mId;
        float factor = static_cast<float> (teamId) / teamCount;
        teamColors[team.mName] = HSVtoRGB( static_cast<int>(360 * factor), 1, 1 );
    }
}

void ClientGui::setPlayerHealth(int health) {

    healthProp.mText = "HP: " + to_string(health);
}

geometry::Point
ClientGui::getEntityMapRefPoint(const geometry::Point &point) const {

    return projectToMapCoords(point);
}

void ClientGui::drawLine(geometry::Point center, Scalar radius,
                         geometry::Angle rotation, const SDL_Color &color) {

    int thickness = 2;
    int radiusInt = static_cast<int>(radius);

    SDL_Rect target = {static_cast<int>(center.x) - thickness / 2,
                       static_cast<int>(center.y) - radiusInt, thickness,
                       radiusInt};

    SDL_Texture *texture =
            SDL_CreateTexture(mRenderer, SDL_GetWindowPixelFormat(mWindow),
                              SDL_TEXTUREACCESS_TARGET, thickness, radius);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(mRenderer, texture);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(mRenderer);
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(mRenderer, nullptr);
    SDL_SetRenderTarget(mRenderer, nullptr);

    SDL_Point ref = {target.w / 2, radiusInt};
    SDL_RenderCopyEx(mRenderer, texture, NULL, &target, rotation, &ref,
                     SDL_RendererFlip::SDL_FLIP_NONE);
}
