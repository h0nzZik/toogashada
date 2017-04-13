#pragma once

#include <vector>
#include <SDL.h>

#include "Point.h"

struct Message;

class ClientGui {
	public:
		ClientGui();
		~ClientGui();

		void run();
		void on_message(Message msg);
		void render_polygon(std::vector<IntPoint> const & points);

	private:
		void handle_event(SDL_Event const & e);
		void handle_user_event(SDL_UserEvent const &e);
		bool quit = false;
		SDL_Window * window;
		SDL_Renderer * renderer;
		uint32_t rcvd_message_event;
};

