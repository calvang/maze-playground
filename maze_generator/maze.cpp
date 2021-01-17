#include <utility>
#include <iostream>
#include <set>
#include <random>
#include <cmath>
#include <algorithm>
#include <list>
#include <iterator>
#include <chrono>

#include "union_find_forest.h"
#include "maze.h"

using namespace std::chrono;

using std::cout;
using std::cerr;
using std::make_pair;
using std::list;


void display_maze(vector<vector<int>>& grid) {
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
 * Generate a random coordinate from the maze cells
 * 
 * @param gen a pseudorandom generator to use
 */ 
template <class URNG>
pair<size_t, size_t> random_coordinate(URNG& gen, size_t width, size_t height) {
    std::uniform_int_distribution<> distr(0, width*height - 1);
    int index = distr(gen); // get random 1D index
    size_t y = index / width;
    size_t x = index - (y * width);
    return make_pair(x, y);
}

/**
 * Generate a random coordinate in the maze grid
 * 
 * @param gen a pseudorandom generator to use
 */ 
template <class URNG>
pair<size_t, size_t> random_maze_coordinate(URNG& gen, size_t width, size_t height) {
    std::uniform_int_distribution<> distr(0, width*height - 1);
    int index = distr(gen); // get random 1D index
    size_t y = index / width;
    size_t x = index - (y * width);
    return make_pair(2 * x + 1, 2 * y + 1); // translate to grid coords
}

/**
 * Generate a random maze using a chosen algorithm
 * 
 * @param algorithm algorithm to use for generation (dfs, kruskal, prim, aldous-broder)
 * @param random_start if true, uses random starting point, overriding startX and startY
 *                     if applicable
 */
unique_ptr<vector<vector<int>>> generate_maze(size_t width, size_t height, string algorithm,
    size_t startX, size_t startY, bool random_start, bool show_frames) {
    if (algorithm == "dfs")
        return randomized_depth_first_search(width, height, startX, startY, random_start, show_frames);
    else if (algorithm == "kruskal")
        return kruskal(width, height, show_frames);
    else if (algorithm == "prim")
        return prim(width, height, startX, startY, random_start, show_frames);
    else if (algorithm == "aldous-broder")
        return aldous_broder(width, height, show_frames);
    else {
        cerr << "ERROR: invalid maze generation algorithm provided!\n";
        return unique_ptr<vector<vector<int>>>{};
    }
}

/**
 * Benchmark a maze generation algorithm
 */
void benchmark_maze(size_t width, size_t height, string algorithm, bool display) {
    auto start = high_resolution_clock::now();
    auto maze = generate_maze(width, height, algorithm);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    if (display) display_maze(*maze);
    cout << algorithm << ": " << duration.count() << " microseconds\n";
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
 * Generate maze using depth-first search
 */
unique_ptr<vector<vector<int>>> randomized_depth_first_search(size_t width, size_t height, 
    size_t startX, size_t startY, bool random_start, bool show_frames) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 3); 
    
    if (random_start) {
        pair<size_t, size_t> start = random_coordinate(gen, width, height);
        startX = start.first;
        startY = start.second;
    }

    vector<vector<int>>* grid = new vector<vector<int>>(width*2+1, vector<int>(height*2+1, 1));
    initialize_grid(*grid);
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
            (*grid)[gridX+neighbor.first][gridY+neighbor.second] = 0;
            visited[x+neighbor.first][y+neighbor.second] = true; 
            stack.push_back(make_pair(x+neighbor.first,y+neighbor.second));
        }
        if (show_frames) display_maze(*grid);
    }
    return unique_ptr<vector<vector<int>>>{grid};
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
 * Generate maze using Kruskal's algorithm
 */
unique_ptr<vector<vector<int>>> kruskal(size_t width, size_t height, bool show_frames) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator

    union_find_forest<pair<size_t,size_t>> cells(width*height); // disjoint set data structure
    vector<pair<size_t,size_t>> walls; // edges, stores pairs of cell indices
    walls.reserve(2*width*height-width-height); // width*(height-1)+(width-1)*height
    initialize_kruskal(cells, walls, width, height);
    shuffle(walls.begin(), walls.end(), gen); // randomize wall order
    
    vector<vector<int>>* grid = new vector<vector<int>>(width*2+1, vector<int>(height*2+1, 1));
    initialize_grid(*grid);
    
    for (size_t i = 0; i < walls.size(); ++i) {
        size_t a = walls[i].first, b = walls[i].second;
        if (cells.union_sets(a, b)) { // remove walls from grid given successful union
            if (cells[a].first < cells[b].first)
                (*grid)[cells[a].first*2+2][cells[a].second*2+1] = 0;
            else if (cells[a].second < cells[b].second)
                (*grid)[cells[a].first*2+1][cells[a].second*2+2] = 0;
        }
        if (show_frames) display_maze(*grid);
    }
    return unique_ptr<vector<vector<int>>>{grid};
}

template <class URNG>
void prim_random_insert(list<size_t>& walls, URNG& gen, size_t value) {
    if (walls.size() > 0) {
        std::uniform_int_distribution<> distr(0, walls.size()-1);
        list<size_t>::iterator position = walls.begin();
        int choice = distr(gen);
        std::advance(position, choice);
        walls.insert(position, value);
    }
    else walls.push_front(value);
}

/**
 * Add current cell's walls to the set
 * 
 * @param position random position to insert in at list
 */
template <class URNG>
void prim_add_walls(vector<vector<int>>& grid, list<size_t>& walls, URNG& gen, 
    size_t grid_width, size_t grid_height, size_t x, size_t y) {
    if (x > 1 && grid[x-2][y] == 1)
        prim_random_insert(walls, gen, grid_width*(x - 1) + y);
    if (y > 1 && grid[x][y-2] == 1)
        prim_random_insert(walls, gen, grid_width*(x) + y - 1);
    if (x < grid_width-2 && grid[x+2][y] == 1)
        prim_random_insert(walls, gen, grid_width*(x + 1) + y);
    if (y < grid_height-2 && grid[x][y+2] == 1)
        prim_random_insert(walls, gen, grid_width*(x) + y + 1);
    grid[x][y] = 0; // mark the cell as visited
} 

/**
 * Generate maze using Prim's algorithm
 */
unique_ptr<vector<vector<int>>> prim(size_t width, size_t height, 
    size_t startX, size_t startY, bool random_start, bool show_frames) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 3);

    if (random_start) {
        pair<size_t, size_t> start = random_coordinate(gen, width, height);
        startX = start.first;
        startY = start.second;
    }

    list<size_t> walls; // edges, stored as grid indices mapped to 1D
    size_t grid_width = width*2+1, grid_height = height*2+1;
    vector<vector<int>>* grid = new vector<vector<int>>(grid_width, vector<int>(grid_height, 1));

    prim_add_walls(*grid, walls, gen, grid_width, grid_height, 2*startX+1, 2*startY+1); // starting pt

    while (walls.size() > 0) {
        size_t current_wall = *(walls.begin());
        size_t wall_x = current_wall / grid_width; // essentially floored
        size_t wall_y = current_wall - (wall_x * grid_width);
        if (wall_x % 2 == 0) { // vertical wall 
            if ((*grid)[wall_x - 1][wall_y] == 1 && (*grid)[wall_x + 1][wall_y] == 0) {
                prim_add_walls(*grid, walls, gen, grid_width, grid_height, wall_x-1, wall_y);
                (*grid)[wall_x][wall_y] = 0;
            }
            else if ((*grid)[wall_x + 1][wall_y] == 1 && (*grid)[wall_x - 1][wall_y] == 0){
                prim_add_walls(*grid, walls, gen, grid_width, grid_height, wall_x+1, wall_y);
                (*grid)[wall_x][wall_y] = 0;
            }
        }
        else { // horizontal wall
            if ((*grid)[wall_x][wall_y - 1] == 1 && (*grid)[wall_x][wall_y + 1] == 0) {
                prim_add_walls(*grid, walls, gen, grid_width, grid_height, wall_x, wall_y-1);
                (*grid)[wall_x][wall_y] = 0;
            }
            else if ((*grid)[wall_x][wall_y + 1] == 1 && (*grid)[wall_x][wall_y - 1] == 0){
                prim_add_walls(*grid, walls, gen, grid_width, grid_height, wall_x, wall_y+1);
                (*grid)[wall_x][wall_y] = 0;
            }
        }
        walls.pop_front();
        if (show_frames) display_maze(*grid);
    }
    return unique_ptr<vector<vector<int>>>{grid};
}

/**
 * Generate maze using Aldous-Broder algorithm
 */
unique_ptr<vector<vector<int>>> aldous_broder(size_t width, size_t height, bool show_frames) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 3); 

    size_t grid_width = width*2+1, grid_height = height*2+1;
    vector<vector<int>>* grid = new vector<vector<int>>(grid_width, vector<int>(grid_height, 1));

    pair<int, int> neighbor_offsets[] = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}}; // offset wrt the grid coords
    size_t unvisited_count = width*height - 1; // start with 1 visited at the start

    pair<size_t, size_t> current = random_maze_coordinate(gen, width, height);
    (*grid)[current.first][current.second] = 0;

    while (unvisited_count > 0) {
        pair<int, int> offset = neighbor_offsets[distr(gen)]; 
        int tmp_x = current.first + offset.first, tmp_y = current.second + offset.second;
        if (tmp_x > 0 && (size_t)tmp_x < grid_width - 1 
            && tmp_y > 0 && (size_t)tmp_y < grid_height - 1) {
            if ((*grid)[tmp_x][tmp_y] == 1) {
                (*grid)[current.first + offset.first / 2][current.second + offset.second / 2] = 0;
                (*grid)[tmp_x][tmp_y] = 0;
                unvisited_count--;
            }
            current.first = tmp_x, current.second = tmp_y;
        }
        if (show_frames) display_maze(*grid);
    }
    return unique_ptr<vector<vector<int>>>{grid};
}