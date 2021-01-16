#include <utility>
#include <iostream>
#include <set>
#include <random>
#include <cmath>
#include <algorithm>
#include <list>
#include <iterator>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
#include "union_find_forest.h"
#include "maze.h"

using std::cout;
using std::pair;
using std::make_pair;
using std::size_t;
using std::list;


void display_grid(vector<vector<int>>& grid) {
    for (size_t j = 0; j < grid[0].size(); ++j) {
        for (size_t i = 0; i < grid.size(); ++i) {
            if (grid[i][j] == 1)
                cout << "█";
            else if (grid[i][j] == 0)
                cout << " ";
        }
        cout << "\n";
    }
}

/**
 * Initialize maze grid with walls between every element
 * Indices containing a 1 are walls
 */
void initialize_grid(vector<vector<int>>& grid) {
    for (size_t i = 1; i < grid.size(); i+=2) {
        for (size_t j = 1; j < grid[0].size(); j+=2) {
            grid[i][j] = 0;
        }
    }
}

/**
 * Generates maze using depth-first search
 */
void randomized_depth_first_search(size_t width, size_t height, size_t startX, size_t startY) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 3); 
    vector<vector<int>> grid(width*2+1, vector<int>(height*2+1, 1)); // init matrix of 1s
    initialize_grid(grid);
    vector<vector<bool>> visited(width, vector<bool>(height, false));
    vector<pair<size_t, size_t>> stack;
    size_t maxX = width-1, maxY = height-1;
    pair<size_t, size_t> current;
    stack.push_back(make_pair(startX, startY));
    while (!stack.empty()) {
        current = stack.back();
        stack.pop_back();
        // check the neighboring cells
        size_t x = current.first, y = current.second;
        // store coefficients to add for the neighbors
        vector<pair<int, int>> unvisited_neighbors;
        unvisited_neighbors.reserve(4);
        if (x > 0 && !visited[x-1][y])
            unvisited_neighbors.push_back(make_pair(-1,0));
        if (x < maxX && !visited[x+1][y])
            unvisited_neighbors.push_back(make_pair(1,0));
        if (y > 0 && !visited[x][y-1])
            unvisited_neighbors.push_back(make_pair(0,-1));
        if (y < maxY && !visited[x][y+1])
            unvisited_neighbors.push_back(make_pair(0,1));
        if (unvisited_neighbors.size() > 0) {
            // choose random neighbor
            int choice = distr(gen);
            while (choice >= (int)unvisited_neighbors.size())
                choice = distr(gen);
            pair<int, int> neighbor = unvisited_neighbors[choice];
            // perform operations on chosen neighbor
            stack.push_back(current);
            size_t gridX = (size_t)(2*x+1), gridY = (size_t)(2*y+1);
            grid[gridX+neighbor.first][gridY+neighbor.second] = 0;
            visited[x+neighbor.first][y+neighbor.second] = true; 
            stack.push_back(make_pair(x+neighbor.first,y+neighbor.second));
        }
    }
    display_grid(grid);
}

/**
 * Initialize grid for kruskal
 * Grid does not have boundaries yet
 */
void initialize_kruskal(union_find_forest<pair<size_t,size_t>>& cells, 
    vector<pair<size_t, size_t>>& walls, size_t width, size_t height) {
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            pair<size_t, size_t> cell(i, j);
            size_t cell_index = cells.size(); // indexed as if it were 1D array
            cells.insert(cell);
            if (i > 0) { // neighbor at i - 1
                size_t neighbor_index = cell_index - height;
                walls.push_back(make_pair(neighbor_index, cell_index));
            }
            if (j > 0) { // neighbor at j - 1
                size_t neighbor_index = cell_index - 1;
                walls.push_back(make_pair(neighbor_index, cell_index));
            }
        }
    }
}

/**
 * Generates maze using Kruskal's algorithm
 */
void kruskal(size_t width, size_t height) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    union_find_forest<pair<size_t,size_t>> cells(width*height); // disjoint set data structure
    vector<pair<size_t,size_t>> walls; // edges, stores pairs of cell indices
    walls.reserve(2*width*height-width-height); // width*(height-1)+(width-1)*height
    initialize_kruskal(cells, walls, width, height);
    shuffle(walls.begin(), walls.end(), gen); // randomize wall order
    vector<vector<int>> grid(width*2+1, vector<int>(height*2+1, 1));
    initialize_grid(grid);
    for (size_t i = 0; i < walls.size(); ++i) {
        size_t a = walls[i].first, b = walls[i].second;
        if (cells.union_sets(a, b)) { // remove walls from grid given successful union
            if (cells[a].first < cells[b].first)
                grid[cells[a].first*2+2][cells[a].second*2+1] = 0;
            else if (cells[a].second < cells[b].second)
                grid[cells[a].first*2+1][cells[a].second*2+2] = 0;
        }
    }
    display_grid(grid);
}

/**
 * Add current cell's walls to the set
 * 
 * @param position random position to insert in at list
 */
template <class URNG>
void prim_add_walls(vector<vector<int>>& grid, list<size_t>& walls, URNG& gen, 
    size_t grid_width, size_t grid_height, size_t x, size_t y) {
    std::uniform_int_distribution<> distr(0, walls.size()); 
    if (x > 1 && grid[x-2][y] == 1) {
        std::list<size_t>::iterator position = walls.begin();
        std::advance(position, distr(gen));
        walls.insert(position, grid_width*(x - 1) + y);
    }
    if (y > 1 && grid[x][y-2] == 1) {
        std::list<size_t>::iterator position = walls.begin();
        std::advance(position, distr(gen));
        walls.insert(position, grid_width*(x) + y - 1);
    }
    if (x < grid_width-1 && grid[x+2][y] == 1) {
        std::list<size_t>::iterator position = walls.begin();
        std::advance(position, distr(gen));
        walls.insert(position, grid_width*(x + 1) + y);
    }
    if (y < grid_height-1 && grid[x][y+2] == 1) {
        std::list<size_t>::iterator position = walls.begin();
        std::advance(position, distr(gen));
        walls.insert(position, grid_width*(x) + y + 1);
    }
    grid[x][y] = 0; // mark the cell as visited
} 

/**
 * Generates maze using Prim's algorithm
 */
void prim(size_t width, size_t height, size_t startX, size_t startY) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 3);
    list<size_t> walls; // edges, stored as grid indices mapped to 1D
    size_t grid_width = width*2+1, grid_height = height*2+1;
    vector<vector<int>> grid(grid_width, vector<int>(grid_height, 1));
    // initialize_grid(grid); // don't need empty cells for this
    cout << grid.size() << "," << grid[0].size() << "\n";
    cout << "HI" << "\n";
    prim_add_walls(grid, walls, gen, grid_width, grid_height, 2*startX+1, 2*startY+1); // starting pt
    while (walls.size() > 0) {
        size_t current_wall = *(walls.begin());
        cout << "WALL" << current_wall << "\n";
        size_t wall_x = current_wall / grid_width; // essentially floored
        size_t wall_y = current_wall - (wall_x * grid_width);
        if (wall_x % 2 == 0) { // vertical wall 
            cout << wall_x << "," << wall_y << "\n";
            if (grid[wall_x - 1][wall_y] == 1) 
                prim_add_walls(grid, walls, gen, grid_width, grid_height, wall_x-1, wall_y);
            else 
                prim_add_walls(grid, walls, gen, grid_width, grid_height, wall_x+1, wall_y);
        }
        else { // horizontal wall
            cout << wall_x << "," << wall_y << ".\n";
            if (grid[wall_x][wall_y - 1] == 1) 
                prim_add_walls(grid, walls, gen, grid_width, grid_height, wall_x, wall_y-1);
            else 
                prim_add_walls(grid, walls, gen, grid_width, grid_height, wall_x, wall_y+1);
        }
        walls.pop_front();
        cout << "SIZE" << walls.size() << "\n";
    }
    display_grid(grid);
}