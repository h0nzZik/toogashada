#include <iostream>
#include <SDL2/SDL.h>
#include <boost/asio.hpp>
#include <string>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void sdl_hello()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		cerr << "SDL could not initialize! SDL_Error:" << SDL_GetError() << endl;
		return;
	}

	auto window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if (!window) {
		cerr << string(SDL_GetError()) << endl;
		return;
	}

    auto screenSurface = SDL_GetWindowSurface( window );
    SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x20, 0x80, 0xFF ) );
    SDL_UpdateWindowSurface( window );
    SDL_Delay( 2000 );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

int main()
{
	cout << "Client\n";
	sdl_hello();
}
