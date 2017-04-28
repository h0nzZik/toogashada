#pragma once

#include <memory>

#include <common/geometry/Point.h>
#include <common/geometry/PolygonalShape.h>

struct DrawProp;
struct Message;

struct EntityComponentSystem;

class ClientGui {
    class Impl;
    std::unique_ptr<Impl> pimpl;

public:
    ClientGui();
    ~ClientGui();

    void render_polygon(geometry::Point center, std::vector<geometry::Point> const &points);

    void renderGui(EntityComponentSystem & entities);

    //void drawRect();

    void drawRect(DrawProp &dp);
};
