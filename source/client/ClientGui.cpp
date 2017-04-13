#include <stdexcept>
#include <string>
#include <iostream>
#include <memory>

// SDL2
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include <common/Tag.h>
#include <common/Message.h>
#include <common/Messages.h>

#include "ClientGui.h"

using namespace std;

ClientGui::ClientGui()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		throw std::runtime_error("Cannot initialize SDL: " + string(SDL_GetError()));

	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if (!window)
		throw std::runtime_error("Cannot create window: " + string(SDL_GetError()));

	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if(!renderer)
		throw std::runtime_error("Cannot create renderer: " + string(SDL_GetError()));
	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
}

ClientGui::~ClientGui() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void ClientGui::update_gui() {
	SDL_UpdateWindowSurface( window );
}

void ClientGui::clear_gui() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}


void ClientGui::render_polygon(IntPoint center, std::vector<IntPoint> const & points) {
	size_t const n = points.size();
	auto xs = make_unique<Sint16[]>(n);
	auto ys = make_unique<Sint16[]>(n);
	for (size_t i = 0; i < n; i++) {
		xs[i] = points[i].x + center.x;
		ys[i] = points[i].y + center.y;
	}
	polygonRGBA(renderer, xs.get(), ys.get(), n, 255, 200, 150, 128);
	SDL_RenderPresent( renderer );
}

