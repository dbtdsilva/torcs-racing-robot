#ifndef MAP_H
#define MAP_H

#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "MapSDL2.h"

typedef enum { SAFE, UNSAFE, UNKNOWN } PositionState;

typedef struct color {
    color() : R(220), G(220), B(220), A(255) {};
    void set(Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
        this->R = R;
        this->G = G;
        this->B = B;
        this->A = A;
    }
    Uint8 R, G, B, A;
} Color;

typedef struct PositionStats {
    PositionStats() : referred(false), wall_counter(0), ground_counter(0), visited(0), state(UNKNOWN) { }
    bool referred;
    int wall_counter, ground_counter;
    unsigned int visited;
    PositionState state;
    Color color;
} Stats;

class Map {
public:
    Map();
    Map(int cols, int rows, int square_precision);

    bool increase_wall_counter(const double& x, const double& y);
    bool increase_ground_counter(const double& x, const double& y);
    bool increase_ground_counter_range(const double& sx, const double& sy, const double& fx, const double& fy);
    bool increase_visited_counter(const double& x, const double& y);

    PositionState get_position_state(const int& x, const int& y) const;
    const std::vector<std::vector<Stats>>& get_raw_reference() const;

    void enable_debug();
    void render_map();
private:
    std::tuple<int, int> convert_to_map_coordinates(const std::tuple<double, double>&) const;
    std::tuple<int, int> convert_to_map_coordinates(const double&, const double&) const;
    std::tuple<double, double> convert_from_map_coordinates(const int&, const int&) const;
    std::tuple<double, double> convert_from_map_coordinates(const std::tuple<int, int>&) const;
    void evaluate_position(const int& x, const int& y);
    bool validate_position(const int& x, const int& y) const;
    bool validate_position(const std::tuple<int, int>&) const;

    const int square_precision_, rows_, cols_;
    std::vector<std::vector<Stats>> map_;
    std::unique_ptr<MapSDL2> map_debug_;
    std::tuple<int, int> last_visited_pos_;

    std::tuple<int, int> min_position, max_position;
    int range_x, range_y;

    std::unique_ptr<std::tuple<int, int>> ptr_objective_;
    std::vector<std::tuple<int, int>> calculated_target_path_, unknown_path_, known_path_;
    std::vector<std::tuple<double, double>> calculated_target_path_converted_;
};


#endif //RAZERNAGA_MAP_H
