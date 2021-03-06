#ifndef MAPSDL2_H
#define MAPSDL2_H

#include <vector>
#include <SDL2/SDL.h>

class Map;

class MapSDL2 {
public:
    MapSDL2(int square_size);
    ~MapSDL2();

    void render_full_map(const Map*);
    bool exit_requested();
private:
    const int square_size_;

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Event events_;
};

#endif //RAZERNAGA_MAPSDL2_H
