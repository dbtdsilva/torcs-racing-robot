#include "MapSDL2.h"
#include "Map.h"
#include <iostream>

using namespace std;

MapSDL2::MapSDL2(int square_size) : square_size_(square_size)
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) return;

    window_ = SDL_CreateWindow("Skylake Vision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              200, 200, SDL_WINDOW_OPENGL);
    if (window_ == nullptr) return;
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) return;
}

MapSDL2::~MapSDL2() {
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

bool MapSDL2::exit_requested() {
    while(SDL_PollEvent(&events_)) {
        if (events_.type == SDL_QUIT)
            return true;
    }
    return false;
}

void MapSDL2::render_full_map(const Map* map) {
    const std::vector<std::vector<Stats>>& raw_map = map->get_raw_reference();

    int width, height;
    SDL_GetWindowSize(window_, &width, &height);
    if (width < raw_map.size() || height < raw_map[0].size())
        SDL_SetWindowSize(window_, raw_map.size(), raw_map[0].size());
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderClear(renderer_);

    SDL_Rect rect;
    for (int x = 0; x < raw_map.size(); x++) {
        for (int y = 0; y < raw_map[x].size(); y++) {
            rect.x = x * (square_size_ + 1);
            rect.y = y * (square_size_ + 1);
            rect.w = square_size_;
            rect.h = square_size_;

            SDL_SetRenderDrawColor(renderer_, raw_map[x][y].color.R, raw_map[x][y].color.G,
                                   raw_map[x][y].color.B, raw_map[x][y].color.A);
            SDL_RenderFillRect(renderer_, &rect);
        }
    }
    SDL_RenderPresent(renderer_);

}