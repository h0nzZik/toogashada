#pragma once

#include <memory>
#include <common/GPoint.h>

struct DrawProp;
struct Message;

struct EntityComponentSystem;

class ClientGui {
    class Impl;
    std::unique_ptr<Impl> pimpl;

public:
    ClientGui();
    ~ClientGui();

    void render_polygon(Point center, std::vector<Point> const &points);

    void renderGui(EntityComponentSystem & entities);

    //void drawRect();

    void drawRect(DrawProp &dp);
};
