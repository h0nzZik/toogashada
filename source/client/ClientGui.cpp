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
#include <common/GameObjectManager.h>
#include <common/EntityComponentSystem.h>

#include "DrawProp.h"

#include "ClientGui.h"

using namespace std;


class ClientGui::Impl {

    /* const */ int SCREEN_WIDTH;
    /* const */ int SCREEN_HEIGHT;

    // GUI settings
    DrawProp mapBoundingBoxRatio {
            {0,0,100,90},
            {}
    };


    enum class Color {
        BG,
        INFO_BG,
    };
    std::map<Color, SDL_Color> mColors = {
            {Color::BG, {0, 0, 0, 255}},
            {Color::INFO_BG, {255, 255, 255, 255}},
    };

    // Computed at init
    DrawProp mapBoundingBox { {}, mColors[Color::BG]};
    DrawProp infoBoundingBox { {}, mColors[Color::INFO_BG]};

    void handle_event(SDL_Event const &e);



    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;

    void drawClearBg(const SDL_Color &color) const {
        SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(mRenderer);
    }

    void drawAppBg() const {
    	drawClearBg(mColors.at(Color::BG));
    }

    void render() const {
    	SDL_RenderPresent(mRenderer);
    }

    void loadMedia() {

    }

    void initGui() {
        {
            auto bbr = mapBoundingBoxRatio;
            auto &bb = mapBoundingBox;

            bb.w() = (bbr.w() * SCREEN_WIDTH)/100;
            bb.h() = (bbr.h() * SCREEN_HEIGHT)/100;
            bb.x() = (bbr.x() * SCREEN_WIDTH)/100;
            bb.y() = (bbr.y() * SCREEN_HEIGHT)/100;
            bb.color = bbr.color;
        }

        {
            auto &mb = mapBoundingBox;
            auto &bb = infoBoundingBox;

            bb.w() = mb.w();
            bb.h() = SCREEN_HEIGHT - mb.h();
            bb.x() = mb.y();
            bb.y() = SCREEN_HEIGHT - mb.h();
        }
    }

public:
    Impl() {
    	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    		throw std::runtime_error("Cannot initialize SDL: " + string(SDL_GetError()));

    	SDL_DisplayMode dm;
    	SDL_GetCurrentDisplayMode(0, &dm);

    	//Screen dimension constants
    	SCREEN_WIDTH = static_cast<int>(dm.w * 0.9);
    	SCREEN_HEIGHT = static_cast<int>(dm.h * 0.9);

    	mWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    	if (!mWindow)
    		throw std::runtime_error("Cannot create window: " + string(SDL_GetError()));

    	mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED );
    	if(!mRenderer)
    		throw std::runtime_error("Cannot create renderer: " + string(SDL_GetError()));

        loadMedia();
        render();

        initGui();
    }
    ~Impl() {
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
    	SDL_Quit();
    }

    void render_polygon(Point center, std::vector<Point> const &points) {
    	size_t const n = points.size();
    	auto xs = make_unique<Sint16[]>(n);
    	auto ys = make_unique<Sint16[]>(n);
    	for (size_t i = 0; i < n; i++) {
    		xs[i] = points[i].x + center.x;
    		ys[i] = points[i].y + center.y;
    	}
    	polygonRGBA(mRenderer, xs.get(), ys.get(), n, 255, 200, 150, 128);
    }

    void renderGui(GameObjectManager &gameObjects, EntityComponentSystem & entities) {
    	using namespace std::placeholders;

    	drawAppBg();

        drawRect(mapBoundingBox);
        drawRect(infoBoundingBox);

#if 0
    	for (const GameObject &go : gameObjects) {

    		render_polygon(go.center, go.shape);
    	}
#endif

    	entities.entityManager.for_each<Shape, Position>(
    			std::bind(&Impl::render_entity,this,_1,_2,_3)
    	);

    	render();
    }

    void render_entity(entity_t const & entity, Shape const & shape, Position const & position) {
    	//cout << "Rendering entity" << endl;
    	//cout << "Rendering: " << shape << endl;
        struct Renderer : public boost::static_visitor<void> {
        	Impl & self;
        	entity_t const & entity;
        	Position const & position;
        	Renderer(Impl & self, entity_t const & entity, Position const & position ) :
        		self(self), entity(entity), position(position)
        	{

        	}

        	void operator()(PolygonalShape const & shape){
        		cout << "Rendering polygon " << endl;
        		self.render_polygon(position.center, shape);
        	}

        	void operator()(CircleShape const & shape) {
        		cout << "Rendering circle " << endl;
        		cout << "radius: " << shape.radius << endl;
        	}
        };
    	Renderer renderer{*this, entity, position};
        boost::apply_visitor(renderer, shape);
        cout << "center: " << position.center << endl;
    }

    //void drawRect();

    void drawRect(DrawProp &dp) {
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, dp.color.r, dp.color.g, dp.color.b, dp.color.a);
        SDL_Rect rect = {dp.x(), dp.y(), dp.w(), dp.h()};
        SDL_RenderFillRect(mRenderer, &rect);
    }
};


ClientGui::ClientGui()
{
	pimpl = make_unique<Impl>();
}

ClientGui::~ClientGui() = default;

void ClientGui::render_polygon(Point center, std::vector<Point> const & points) {
	pimpl->render_polygon(center, points);
}

void ClientGui::renderGui(GameObjectManager &gameObjects, EntityComponentSystem & entities) {
	pimpl->renderGui(gameObjects, entities);
}

void ClientGui::drawRect(DrawProp &dp) {
	pimpl->drawRect(dp);
}
