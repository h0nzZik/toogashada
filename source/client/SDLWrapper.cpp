#include <cassert>

#include <memory>
#include <stdexcept>

#include <SDL.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_system.h>
#include <SDL_video.h>

#include "SDLWrapper.h"

namespace SDLw {

SDL::SDL(uint32_t flags) {
  if (SDL_Init(flags) < 0)
    throw std::runtime_error("Cannot initialize SDL: " +
                             std::string(SDL_GetError()));
}

SDL::~SDL() { SDL_Quit(); }

extern "C" int TTF_Init();
extern "C" void TTF_Quit();

TTF::TTF() {
  if (TTF_Init() < 0)
    throw std::runtime_error("Cannot initialize TTF");
}

TTF::~TTF() { TTF_Quit(); }

} // namespace SDLw

#if 0
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Geometry.h"
#include "SDLWrapper.h"

MySDL::Window::Window(std::string const &title, int width, int height)
    : window_(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED, width,
          height, SDL_WINDOW_SHOWN)) {
	if (!window_)
		throw std::runtime_error("Cannot create window: " + std::string(SDL_GetError()));
}

MySDL::Window::~Window() {
	assert(window_);
	SDL_DestroyWindow(window_);
}

Vector MySDL::Window::size() const {
	Vector vec;
	SDL_GetWindowSize(window_, &vec.x, &vec.y);
	return vec;
}

MySDL::Renderer::Renderer(Window &window)
    : Renderer(*window.get()) {
}

MySDL::Renderer::Renderer(SDL_Window &window)
    : renderer_(SDL_CreateRenderer(&window, -1, SDL_RENDERER_ACCELERATED)) {
	if (!renderer_)
		throw std::runtime_error("Cannot create renderer: " + std::string(SDL_GetError()));
}

MySDL::Renderer::~Renderer() {
	assert(renderer_);
	SDL_DestroyRenderer(renderer_);
}

void MySDL::Renderer::setColor(Rgb rgb) {
	setColor(rgb.r, rgb.g, rgb.b, 255);
}
void MySDL::Renderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}

void MySDL::Renderer::clear() {
	SDL_RenderClear(renderer_);
}

void MySDL::Renderer::render() {
	SDL_RenderPresent(renderer_);
}

void MySDL::Renderer::drawLine(int x0, int y0, int x1, int y1) {
	SDL_RenderDrawLine(renderer_, x0, y0, x1, y1);
}

MySDL::Surface::Surface(std::string const &filename) {
	int required_format = STBI_rgb_alpha;
	int width, height, orig_format;
	data_ = stbi_load(filename.c_str(), &width, &height, &orig_format, required_format);

	if (data_ == nullptr) {
		throw std::runtime_error("Cannot load image '" + filename + "': " + stbi_failure_reason());
	}

	Uint32 rmask, gmask, bmask, amask;
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;

	int depth = 32;
	int pitch = 4 * width;

	surface_ = SDL_CreateRGBSurfaceFrom(static_cast<void *>(data_), width, height, depth, pitch,
	    rmask, gmask, bmask, amask);

	if (surface_ == nullptr) {
		stbi_image_free(data_);
		throw std::runtime_error("Cannot load image '" + filename + "': " + SDL_GetError());
	}
}

MySDL::Surface::Surface(SDL_Surface *surface)
    : surface_(surface) {
	if (!surface_)
		throw std::runtime_error("Surface::Surface(SDL_Surface * surface): parameter cannot be null");
}

MySDL::Surface::~Surface() {
	if (surface_)
		SDL_FreeSurface(surface_);
	if (data_)
		stbi_image_free(data_);
}

MySDL::Surface::Surface(Surface &&old) noexcept
    : surface_(nullptr) {
	std::swap(surface_, old.surface_);
}

Vector MySDL::Surface::size() const {
	assert(surface_);
	return { surface_->w, surface_->h };
}

MySDL::Texture::~Texture() {
	if (texture_)
		SDL_DestroyTexture(texture_);
}

MySDL::Texture::Texture(Renderer &renderer, Surface &surface)
    : texture_(SDL_CreateTextureFromSurface(renderer.get(), surface.get())) {
	if (!texture_)
		throw std::runtime_error("Cannot create texture from surface");
}

MySDL::Texture::Texture(Texture &&old) noexcept
    : texture_(old.texture_) {
	old.texture_ = nullptr;
}

MySDL::Texture &MySDL::Texture::operator=(Texture &&old) noexcept {
	texture_ = old.texture_;
	old.texture_ = nullptr;
	return *this;
}

Vector MySDL::Texture::size() const {
	Vector v;
	int const rv = SDL_QueryTexture(texture_, nullptr, nullptr, &v.x, &v.y);
	if (rv != 0)
		throw std::runtime_error(std::string("SDL_QueryTexture failed: ") + SDL_GetError());
	return v;
}

MySDL::Texture MySDL::loadTexture(Renderer &renderer, std::string const &filename) {
	Surface surface{ filename };
	return { renderer, surface };
}

SDL_Rect toSDL(Rectangle const &rect) {
	SDL_Rect r;
	r.x = rect.from.x;
	r.y = rect.from.y;
	r.w = rect.to.x - rect.from.x;
	r.h = rect.to.y - rect.from.y;
	return r;
}

void draw(MySDL::Renderer &renderer, MySDL::Texture &texture, Rectangle const &rectangle, double angle) {
	SDL_Rect rect = toSDL(rectangle);
	SDL_RenderCopyEx(renderer.get(), texture.get(), nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
}

#endif
