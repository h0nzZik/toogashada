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

using namespace std;
using namespace geometry;

// TODO standalone class for game area


ClientGui::ClientGui() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("Cannot initialize SDL: " + string(SDL_GetError()));

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    //Screen dimension constants
    SCREEN_WIDTH = static_cast<int>(dm.w * 0.9);
    SCREEN_HEIGHT = static_cast<int>(dm.h * 0.9);

    cout << "Width: " << SCREEN_WIDTH << " Height: " << SCREEN_HEIGHT << endl;

    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!mWindow)
        throw std::runtime_error("Cannot create window: " + string(SDL_GetError()));

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer)
        throw std::runtime_error("Cannot create renderer: " + string(SDL_GetError()));

    loadMedia();
    render();

    initGui();
}

ClientGui::~ClientGui() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

Scalar ClientGui::scaleToMapCoords(Scalar coord) {

    return coord * (mapProp.w() / mapRatio.w());
}

geometry::Point ClientGui::projectToMapCoords(geometry::Point point) {

    return {mapProp.x() + scaleToMapCoords(point.x),
            mapProp.y() + scaleToMapCoords(point.y)};
}

void ClientGui::drawClearBg(const SDL_Color &color) const {

    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(mRenderer);
}

void ClientGui::drawAppBg() const {
    drawClearBg(mColors.at(Color::BG));
}

void ClientGui::render() const {
    SDL_RenderPresent(mRenderer);
}

void ClientGui::loadMedia() {

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
            mp.y() = mbb.y() + ((mbb.h() - mp.h())/2);

        } else {

            mp.h() = mbb.h();
            mp.w() = static_cast<int>(mp.h() * mapRatio);
            mp.x() = mbb.x() + ((mbb.w() - mp.w())/2);
            mp.y() = mbb.y();

        }
    }
}

void ClientGui::drawPolygon(Point center, std::vector<Vector> const &points) {

    size_t const n = points.size();
    auto xs = make_unique<Sint16[]>(n);
    auto ys = make_unique<Sint16[]>(n);

    for (size_t i = 0; i < n; i++) {
        geometry::Point point = projectToMapCoords(center + points[i]);
        xs[i] = point.x;
        ys[i] = point.y;
    }

    polygonRGBA(mRenderer, xs.get(), ys.get(), n, 255, 200, 150, 128);

}

void ClientGui::renderGui(EntityComponentSystem &entities) {
    using namespace std::placeholders;

    drawAppBg();
    drawRect(mapBoundingBox);
    drawRect(infoBoundingBox);
    drawRect(mapProp);

    // reference point for mouse testing - hardcoded
    drawRect({{499,499,2,2},mColors[Color::TEST]});

    entities.entityManager.for_each<Shape, Position>(
            std::bind(&ClientGui::drawEntity, this, _1, _2, _3)
    );

    render();
}

void ClientGui::drawEntity(entity_t const &entity, Shape const &shape, Position const &position) {
    struct Drawer : public boost::static_visitor<void> {
        ClientGui &self;
        entity_t const &entity;
        Position const &position;

        Drawer(ClientGui &self, entity_t const &entity, Position const &position) :
                self(self), entity(entity), position(position) {}

        void operator()(PolygonalShape const &shape) {
            self.drawPolygon(position.center, shape);
        }

        void operator()(CircleShape const &shape) {
            Point center = self.projectToMapCoords(position.center);
            filledCircleRGBA(self.mRenderer, center.x, center.y, self.scaleToMapCoords(shape.radius), 0, 0, 255, 255);
        }
    };
    Drawer drawer{*this, entity, position};
    boost::apply_visitor(drawer, shape);
}

//void drawRect();

void ClientGui::drawRect(const DrawProp &dp) {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, dp.color.r, dp.color.g, dp.color.b, dp.color.a);
    SDL_Rect rect = {dp.x(), dp.y(), dp.w(), dp.h()};
    SDL_RenderFillRect(mRenderer, &rect);
}

void ClientGui::setMapSize(int w, int h) {

    mapRatio.w() = w;
    mapRatio.h() = h;

    initGui();
}
