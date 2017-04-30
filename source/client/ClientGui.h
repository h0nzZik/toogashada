#pragma once

#include <memory>

struct DrawProp;

struct EntityComponentSystem;

class ClientGui {
    class Impl;
    std::unique_ptr<Impl> pimpl;

public:
    ClientGui();
    ~ClientGui();

    void renderGui(EntityComponentSystem & entities);

    //void drawRect();

    void drawRect(DrawProp &dp);
};
