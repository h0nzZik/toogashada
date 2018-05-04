#include <cassert>

#include <memory>
#include <stdexcept>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_system.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include "Geometry.h"
#include "SDLWrapper.h"

MySDL::SDL::SDL(uint32_t flags) {
  if (SDL_Init(flags) < 0)
    throw std::runtime_error("Cannot initialize SDL: " +
                             std::string(SDL_GetError()));
}

MySDL::SDL::~SDL() { SDL_Quit(); }

MySDL::TTF::TTF() {
  if (TTF_Init() < 0)
    throw std::runtime_error("Cannot initialize TTF");
}

MySDL::TTF::~TTF() { TTF_Quit(); }

MySDL::Window::Window(std::string const &title, int width, int height)
    : window_(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, width, height,
                               SDL_WINDOW_SHOWN)) {
  if (!window_)
    throw std::runtime_error("Cannot create window: " +
                             std::string(SDL_GetError()));
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

MySDL::Renderer::Renderer(Window &window) : Renderer(*window.get()) {}

MySDL::Renderer::Renderer(SDL_Window &window)
    : renderer_(SDL_CreateRenderer(&window, -1, SDL_RENDERER_ACCELERATED)) {
  if (!renderer_)
    throw std::runtime_error("Cannot create renderer: " +
                             std::string(SDL_GetError()));
}

MySDL::Renderer::~Renderer() {
  assert(renderer_);
  SDL_DestroyRenderer(renderer_);
}

void MySDL::Renderer::setColor(Rgb rgb) { setColor(rgb.r, rgb.g, rgb.b, 255); }
void MySDL::Renderer::setColor(int r, int g, int b, int a) {
  SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}

void MySDL::Renderer::clear() { SDL_RenderClear(renderer_); }

void MySDL::Renderer::render() { SDL_RenderPresent(renderer_); }

void MySDL::Renderer::drawLine(int x0, int y0, int x1, int y1) {
  SDL_RenderDrawLine(renderer_, x0, y0, x1, y1);
}

MySDL::Surface::Surface(std::string const &filename)
    : surface_(IMG_Load(filename.c_str())) {
  if (!surface_)
    throw std::runtime_error("Cannot load image '" + filename +
                             "': " + std::string(IMG_GetError()));
}

MySDL::Surface::Surface(SDL_Surface *surface) : surface_(surface) {
  if (!surface_)
    throw std::runtime_error(
        "Surface::Surface(SDL_Surface * surface): parameter cannot be null");
}

MySDL::Surface::~Surface() {
  if (surface_)
    SDL_FreeSurface(surface_);
}

MySDL::Surface::Surface(Surface &&old) : surface_(nullptr) {
  std::swap(surface_, old.surface_);
}

Vector MySDL::Surface::size() const {
  assert(surface_);
  return {surface_->w, surface_->h};
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

MySDL::Texture::Texture(Texture &&old) : texture_(old.texture_) {
  old.texture_ = nullptr;
}

MySDL::Texture &MySDL::Texture::operator=(Texture &&old) {
  texture_ = old.texture_;
  old.texture_ = nullptr;
  return *this;
}

Vector MySDL::Texture::size() const {
  Vector v;
  int const rv = SDL_QueryTexture(texture_, nullptr, nullptr, &v.x, &v.y);
  if (rv != 0)
    throw std::runtime_error(std::string("SDL_QueryTexture failed: ") +
                             SDL_GetError());
  return v;
}

MySDL::Texture MySDL::loadTexture(Renderer &renderer,
                                  std::string const &filename) {
  Surface surface{filename};
  return {renderer, surface};
}

MySDL::Font::Font(std::string const &file, int pointsize)
    : font_(TTF_OpenFont(file.c_str(), pointsize)) {
  if (!font_)
    throw std::runtime_error("Cannot open font file " + file + ": " +
                             std::string(TTF_GetError()));
}

MySDL::Font::~Font() {
  assert(font_);
  TTF_CloseFont(font_);
}

MySDL::Surface MySDL::Font::renderText_solid(std::string const &text,
                                             Rgb color) {
  return {TTF_RenderText_Blended(font_, text.c_str(),
                                 {color.r, color.g, color.b, 255})};
}

SDL_Rect toSDL(Rectangle const &rect) {
  SDL_Rect r;
  r.x = rect.from.x;
  r.y = rect.from.y;
  r.w = rect.to.x - rect.from.x;
  r.h = rect.to.y - rect.from.y;
  return r;
}

void draw(MySDL::Renderer &renderer, MySDL::Texture &texture,
          Rectangle const &rectangle, double angle) {
  SDL_Rect rect = toSDL(rectangle);
  SDL_RenderCopyEx(renderer.get(), texture.get(), nullptr, &rect, angle,
                   nullptr, SDL_FLIP_NONE);
}
