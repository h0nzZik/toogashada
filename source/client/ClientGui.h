#pragma once

#include <memory>
#include <common/GPoint.h>

struct DrawProp;
struct Message;
class GameObject;
template < typename T > class GenericManager;
using GameObjectManager = GenericManager<GameObject>;

struct EntityComponentSystem;

class ClientGui {
    class Impl;
    std::unique_ptr<Impl> pimpl;

public:
    ClientGui();
    ~ClientGui();

    void render_polygon(Point center, std::vector<Point> const &points);

    void renderGui(GameObjectManager &gameObjects, EntityComponentSystem & entities);

    //void drawRect();

    void drawRect(DrawProp &dp);
};
