#include <utility>
#include <iostream>
#include <set>
#include <random>
#include <cmath>
#include <algorithm>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
#include "union_find_forest.h"
#include "maze.h"

using std::cout;
using std::pair;
using std::make_pair;
using std::size_t;


void display_grid(vector<vector<int>>& grid){
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
void initialize_grid(vector<vector<int>>& grid){
    for (size_t i = 1; i < grid.size(); i+=2) {
        for (size_t j = 1; j < grid[0].size(); j+=2) {
            grid[i][j] = 0;
        }
    }
}

/**
 * Generates maze using depth-first search
 */
void randomized_depth_first_search(size_t width, size_t height){
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 3); 
    vector<vector<int>> grid(width*2+1, vector<int>(height*2+1, 1)); // init matrix of 1s
    initialize_grid(grid);
    vector<vector<bool>> visited(width, vector<bool>(height, false));
    vector<pair<size_t, size_t>> stack;
    size_t startX = 0, startY = 0;
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
            size_t cell_index = cells.size();
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
void kruskal(size_t width, size_t height){
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    union_find_forest<pair<size_t,size_t>> cells(width*height); // disjoint set data structure
    vector<pair<size_t,size_t>> walls; // edges
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