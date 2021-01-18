#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>
#include "path.h"

using std::cout;
using std::cerr;
using std::pair;
using std::make_pair;
using std::priority_queue;
using std::unordered_map;

/**
 * Print maze and path to stdout
 * 
 * @param colors enable colors for terminals that support it
 * @param visited indicate visited cells even if they are not on the path
 */ 
void display_path(vector<vector<int>>& grid, bool colors, bool visited) {
    unordered_map<int, string> blocks = {
        {0, "  "},
        {1, "██"},
        {2, "◀▶"},
        {3, "SS"},
        {4, "EE"},
        {5, "  "}};
    if (visited) {
        if (colors) blocks[5] = "\033[1;34m██\033[0m";
        else blocks[5] = "..";
    }
    if (colors) {
        blocks[2] = "\033[1;35m██\033[0m";
        blocks[3] = "\033[1;31m██\033[0m";
        blocks[4] = "\033[1;32m██\033[0m";
    }
    for (size_t j = 0; j < grid[0].size(); ++j) {
        for (size_t i = 0; i < grid.size(); ++i)
            cout << blocks[grid[i][j]];
        cout << "\n";
    }
}

/**
 * Save maze with path to a file
 */ 
void save_path(vector<vector<int>>& grid, string file_path) {
    std::ofstream outfile(file_path);
    if (!outfile.is_open()) {
        cerr << "ERROR: unable to open file!\n";
        exit(1);
    }
    for (size_t j = 0; j < grid[0].size(); ++j) {
        for (size_t i = 0; i < grid.size(); ++i) {
            outfile << std::to_string(grid[i][j]);
        }
        outfile << "\n";
    }
}

/**
 * Open a saved maze and path from a file
 */ 
unique_ptr<vector<vector<int>>> load_path(string file_path) {
    std::ifstream infile(file_path);
    if (!infile.is_open()) {
        cerr << "ERROR: unable to open file!\n";
        exit(1);
    }
    string line;
    bool is_first = true;
    vector<vector<int>>* grid = new vector<vector<int>>();
    while (std::getline(infile, line)) {
        size_t row = 0;
        for (char cell : line) {
            if (is_first) grid->push_back(vector<int>());
            (*grid)[row].push_back(cell - '0');
            row++;
        }
        if (is_first) is_first = false;
    }
    return unique_ptr<vector<vector<int>>>{grid};
}

size_t flatten_coordinate(size_t width, size_t x, size_t y) {
    return width * y + x;
}

pair<size_t, size_t> restore_coordinate(size_t width, size_t flattened_coord) {
    size_t y = flattened_coord / width; // essentially floored
    size_t x = flattened_coord - (y * width);
    return make_pair(x, y);
}

double a_star_heuristic(size_t x, size_t y, size_t goalX, size_t goalY, int heuristic) {
    // manhattan distance
    if (heuristic == 0) 
        return double(std::abs((int)x - (int)goalX) + std::abs((int)y - (int)goalY));
    else return 0;
}

struct a_star_cell {
    double g_score;
    double f_score;
    size_t x;
    size_t y;
    size_t index;
    bool in_queue; // true if in priority queue
    // for the start point
    a_star_cell() : g_score(std::numeric_limits<double>::max()), 
        f_score(std::numeric_limits<double>::max()), x(0), y(0), index(0), in_queue(false) {}
    a_star_cell(size_t x, size_t y, size_t endX, size_t endY, size_t width, int heuristic) 
        : g_score(0), f_score(a_star_heuristic(x, y, endX, endY, heuristic)), 
        x(x), y(y), index(flatten_coordinate(width, x, y)), in_queue(true) {}
    // for other points
    a_star_cell(size_t index, size_t width) : g_score(std::numeric_limits<double>::max()), 
        f_score(std::numeric_limits<double>::max()), index(index), in_queue(false) {
        auto coords = restore_coordinate(width, index);
        x = coords.first, y = coords.second;
    }
};

struct f_score_cmp {
    bool operator()(const a_star_cell* a, const a_star_cell* b) {
        return a->f_score < b->f_score;
    }
};

/**
 * Solves maze using A* algorithm
 * Coordinates are wrt the number of cells (input to generate_maze)
 * 
 * @param heuristic heuristic to use (manhattan, euclidean)
 */ 
bool a_star(vector<vector<int>>& grid, size_t startX, size_t startY, size_t endX, size_t endY,
    string heuristic, bool track_visited) {
    // determine heuristic to use
    int heuristic_index = 0;
    if (heuristic == "euclidean") heuristic_index = 1;
    else if (heuristic != "manhattan") cerr << "ERROR: Invalid A* heuristic!\n";

    size_t width = (grid.size()-1)/2, height = (grid[0].size()-1)/2;
    int neighbor_offsets[] = {-1, -1 * (int) width, +1, (int) width}; // index offsets
    pair<int, int> neighbor_grid_offsets[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}}; // grid offsets / 2
    double wall_weight = 1;

    // Coordinates will be referenced with 1D indices
    vector<a_star_cell> scores; // tracks g and f scores
    scores.reserve(width*height);
    for (size_t i = 0; i < width*height; ++i) scores.push_back(a_star_cell(i, width));

    // tracks set of discovered cells, ordered by f_score
    priority_queue<a_star_cell*, vector<a_star_cell*>, f_score_cmp> discovered;
    unordered_map<size_t, size_t> predecessors; // tracks preceding cell of each cell

    // handle starting point
    size_t start = flatten_coordinate(width, startX, startY);
    scores[start] = a_star_cell(startX, startY, endX, endY, width, heuristic_index);
    discovered.push(&(scores[start]));

    while (!discovered.empty()) {
        auto current_cell = discovered.top();
        // if reached end, reconstruct path
        if (current_cell->x == endX && current_cell->y == endY) {
            int gridX = (int) 2*endX+1, gridY = (int) 2*endY+1;
            grid[gridX][gridY] = 3;
            size_t current = current_cell->index;
            // backtrack through predecessors
            while (current != start) {
                gridX = (int) 2*scores[current].x+1;
                gridY = (int) 2*scores[current].y+1;
                size_t predecessor = predecessors[current];
                int wallX = (int) scores[predecessor].x - (int) scores[current].x;
                int wallY = (int) scores[predecessor].y - (int) scores[current].y;
                grid[gridX][gridY] = 2;
                grid[gridX + wallX][gridY + wallY] = 2;
                current = predecessor;
            }
            grid[2*startX+1][2*startY+1] = 3;
            grid[2*endX+1][2*endY+1] = 4;
            return true;
        }
        // if (track_visited) grid[2*current_cell->x+1][2*current_cell->y+1] = 5;
        current_cell->in_queue = false;
        discovered.pop();
        for (size_t i = 0; i < 4; ++i) {
            // index offset between current and neighbor
            int offset = (int) current_cell->index + neighbor_offsets[i];
            // grid coords of wall between current and neighbor
            int wallX = 2* (int) current_cell->x + 1 + neighbor_grid_offsets[i].first;
            int wallY = 2* (int) current_cell->y + 1 + neighbor_grid_offsets[i].second;
            // grid coords of neighbor
            int gridX = 2* ((int) current_cell->x + neighbor_grid_offsets[i].first) + 1;
            int gridY = 2* ((int) current_cell->y + neighbor_grid_offsets[i].second) + 1;
            // check that neighbor exists and there is an open path
            if (gridX > 0 && gridX < 2 * (int) width && gridY > 0 && gridY < 2 * (int) height
                && grid[wallX][wallY] != 1) {
                double new_g_score = current_cell->g_score + wall_weight;
                if (new_g_score < scores[offset].g_score) {
                    // mark visited neighbors and walls if applicable
                    if (track_visited) {
                        grid[wallX][wallY] = 5;
                        grid[gridX][gridY] = 5;
                    }
                    // if neighbor has better score, move there
                    predecessors[offset] = current_cell->index;
                    scores[offset].g_score = new_g_score;
                    scores[offset].f_score = new_g_score + a_star_heuristic(scores[offset].x,
                        scores[offset].y, endX, endY, heuristic_index);
                    if (!scores[offset].in_queue) {
                        discovered.push(&(scores[offset]));
                        scores[offset].in_queue = true;
                    }
                }
            }
        }    
    }
    cerr << "No path found!\n";
    return false;
}
