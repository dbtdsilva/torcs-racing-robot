#include "Map.h"
#include <iostream>
#include <algorithm>

#include "SkylakeConsts.h"

using namespace std;

Map::Map() : map_(1000, vector<Stats>(1000, Stats())),
        cols_(1000), rows_(1000), square_precision_(4), last_visited_pos_(convert_to_map_coordinates(0,0)),
        ptr_objective_(nullptr), min_position(-500, -500), max_position(500, 500),
        range_x(1000), range_y(1000) {
}

void Map::enable_debug() {
    if (map_debug_ == nullptr)
        map_debug_ = make_unique<MapSDL2>(4);
}

bool Map::increase_wall_counter(const double& x, const double& y) {
    tuple<int, int> position = convert_to_map_coordinates(x, y);
    if (!validate_position(position)) return false;
    map_[M_X(position)][M_Y(position)].wall_counter++;
    evaluate_position(M_X(position), M_Y(position));
    return true;
}

bool Map::increase_ground_counter(const double& x, const double& y) {
    tuple<int, int> position = convert_to_map_coordinates(x, y);
    if (!validate_position(position)) return false;
    map_[M_X(position)][M_Y(position)].ground_counter++;
    evaluate_position(M_X(position), M_Y(position));
    return true;
}

bool Map::increase_ground_counter_range(const double& sx, const double& sy, const double& fx, const double& fy) {
    long double dx, dy;
    const int N_POINTS = 100;

    dx = (fx - sx) / N_POINTS;
    dy = (fy - sy) / N_POINTS;

    vector<Stats*> modified_points;
    for (int points = 0; points < N_POINTS; points++) {
        tuple<int, int> position = convert_to_map_coordinates(sx + points * dx, sy + points * dy);
        if (map_[M_X(position)][M_Y(position)].referred)
            continue;
        map_[M_X(position)][M_Y(position)].referred = true;
        modified_points.push_back(&map_[M_X(position)][M_Y(position)]);
        increase_ground_counter(sx + points * dx, sy + points * dy);
    }

    for (Stats* element : modified_points)
        element->referred = false;
}

bool Map::increase_visited_counter(const double& x, const double& y) {
    tuple<int, int> position = convert_to_map_coordinates(x, y);
    //cout << "A: " << M_X(position) << ", " << M_Y(position) << endl;
    if (!validate_position(position)) return false;
    //cout << "Valid!" << endl;
    map_[M_X(position)][M_Y(position)].visited++;

    M_X(last_visited_pos_) = M_X(position);
    M_Y(last_visited_pos_) = M_Y(position);
    evaluate_position(M_X(position), M_Y(position));
    return true;
}

void Map::evaluate_position(const int& x, const int& y) {
    if (map_[x][y].visited > 0)
        map_[x][y].state = SAFE;
    else {
        map_[x][y].state = map_[x][y].wall_counter <= map_[x][y].ground_counter *
                                                              SkylakeConsts::MAP_SAFE_MARGIN ? SAFE : UNSAFE;
    }

    if (map_debug_ != nullptr) {
        if (map_[x][y].state == SAFE)
            map_[x][y].color.set(0, 255, 0, 255);
        else if (map_[x][y].state == UNSAFE)
            map_[x][y].color.set(0, 0, 0, 255);
    }
}

void Map::render_map() {
    if (map_debug_ == nullptr)
        return;

    if (map_debug_->exit_requested()) {
        cout << "RazerNaga has been requested to exit!" << endl;
        std::exit(0);
    }

    vector<tuple<int, int, Uint8, Uint8, Uint8, Uint8>> temporary_paintings;
    Color color;
    for (auto path_node : unknown_path_) {
        color = map_[M_X(path_node)][M_Y(path_node)].color;
        temporary_paintings.push_back(tuple<int, int, Uint8, Uint8, Uint8, Uint8>(
                M_X(path_node), M_Y(path_node), color.R, color.G, color.B, color.A));
        map_[M_X(path_node)][M_Y(path_node)].color.set(255, 0, 255, 255);
    }

    for (auto path_node : known_path_) {
        color = map_[M_X(path_node)][M_Y(path_node)].color;
        temporary_paintings.push_back(tuple<int, int, Uint8, Uint8, Uint8, Uint8>(
                M_X(path_node), M_Y(path_node), color.R, color.G, color.B, color.A));
        map_[M_X(path_node)][M_Y(path_node)].color.set(255, 128, 0, 255);
    }

    // Paint the path to the objective location
    for (auto path_node : calculated_target_path_) {
        color = map_[M_X(path_node)][M_Y(path_node)].color;
        temporary_paintings.push_back(tuple<int, int, Uint8, Uint8, Uint8, Uint8>(
                M_X(path_node), M_Y(path_node), color.R, color.G, color.B, color.A));
        map_[M_X(path_node)][M_Y(path_node)].color.set(255, 0, 0, 255);
    }
    // Paint the current position
    color = map_[M_X(last_visited_pos_)][M_Y(last_visited_pos_)].color;
    map_[M_X(last_visited_pos_)][M_Y(last_visited_pos_)].color.set(0, 0, 255, 255);
    temporary_paintings.push_back(tuple<int, int, Uint8, Uint8, Uint8, Uint8>(
            M_X(last_visited_pos_), M_Y(last_visited_pos_), color.R, color.G, color.B, color.A));

    // Render the map
    map_debug_->render_full_map(this);

    // Give map the previous color before entering the current position and A*
    while (!temporary_paintings.empty()) {
        auto &value = temporary_paintings.back();
        temporary_paintings.pop_back();
        map_[get<0>(value)][get<1>(value)].color.set(get<2>(value), get<3>(value), get<4>(value), get<5>(value));
    }
}

bool Map::validate_position(const int& x, const int& y) const {
    return x >= 0 && x < range_x &&
           y >= 0 && y < range_y;
}

bool Map::validate_position(const tuple<int, int>& value) const {
    return validate_position(M_X(value), M_Y(value));
}

PositionState Map::get_position_state(const int& x, const int& y) const {
    return validate_position(x, y) ? map_[x][y].state : UNSAFE;
}

std::tuple<int, int> Map::convert_to_map_coordinates(const std::tuple<double, double>& real_coordinates) const {
    return convert_to_map_coordinates(M_X(real_coordinates), M_Y(real_coordinates));
}

std::tuple<int, int> Map::convert_to_map_coordinates(const double& x, const double& y) const {
    return tuple<int, int>(static_cast<int>(round(x) + range_x / 2.0),
                           static_cast<int>(round(-y)+ range_y / 2.0));
}

std::tuple<double, double> Map::convert_from_map_coordinates(const int& x, const int& y) const {
    return tuple<double, double>((static_cast<double>(x / 10.0) / square_precision_ - cols_),
                                 -SkylakeConsts::MAP_SQUARE_SIZE * (static_cast<double>(y) / square_precision_ - rows_));
}

std::tuple<double, double> Map::convert_from_map_coordinates(const std::tuple<int, int>& map_coordinates) const {
    return convert_from_map_coordinates(M_X(map_coordinates), M_Y(map_coordinates));
}

const std::vector<std::vector<Stats>> &Map::get_raw_reference() const {
    return map_;
}
