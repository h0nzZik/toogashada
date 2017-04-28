#pragma once

#include <memory>
#include <common/GPoint.h>

struct DrawProp;
struct Message;
class GameObject;
template < typename T > class GenericManager;
using GameObjectManager = GenericManager<GameObject>;

class ClientGui {
    class Impl;
    std::unique_ptr<Impl> pimpl;

public:
    ClientGui();
    ~ClientGui();

    void render_polygon(Point center, std::vector<Point> const &points);

    void renderGui(GameObjectManager &gameObjects);

    //void drawRect();

    void drawRect(DrawProp &dp);
};
