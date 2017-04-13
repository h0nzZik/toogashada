#pragma once

#include <vector>
#include <SDL.h>

#include "Point.h"

struct Message;

class ClientGui {
	public:
		ClientGui();
		~ClientGui();

		void clear_gui();
		void update_gui();
		void render_polygon(IntPoint center, std::vector<IntPoint> const & points);

	private:
		void handle_event(SDL_Event const & e);
		SDL_Window * window;
		SDL_Renderer * renderer;
};

