#pragma once

#include <common/GPoint.h>
#include <vector>
#include <SDL.h>


struct Message;

class ClientGui {
	public:
		ClientGui();
		~ClientGui();

		void clear_gui();
		void update_gui();
		void render_polygon(Point center, std::vector<Point> const & points);

	private:
		void handle_event(SDL_Event const & e);
		SDL_Window * window;
		SDL_Renderer * renderer;
};

