#include <vector>
#include <string>
#include <memory>

using std::pair;
using std::vector;
using std::size_t;
using std::unique_ptr;
using std::string;


void display_maze(vector<vector<int>>& grid);

void save_maze(vector<vector<int>>& grid, string file_path, bool binary=false);

unique_ptr<vector<vector<int>>> open_maze(string file_path, bool display=false);

template <class URNG>
pair<size_t, size_t> random_maze_coordinate(URNG& gen, size_t width, size_t height);

unique_ptr<vector<vector<int>>> randomized_depth_first_search(size_t width, size_t height, 
    size_t startX=0, size_t startY=0, bool random_start=true, bool show_frames=false);

unique_ptr<vector<vector<int>>> kruskal(size_t width, size_t height, bool show_frames=false);

unique_ptr<vector<vector<int>>> prim(size_t width, size_t height, size_t startX=0, 
    size_t startY=0, bool random_start=true, bool show_frames=false);

unique_ptr<vector<vector<int>>> aldous_broder(size_t width, size_t height, bool show_frames=false);

unique_ptr<vector<vector<int>>> recursive_division(size_t width, size_t height, bool show_frames=false);

unique_ptr<vector<vector<int>>> generate_maze(size_t width, size_t height, string algorithm="aldous-broder",
    size_t startX=0, size_t startY=0, bool random_start=true, bool show_frames=false);

void benchmark_maze(size_t width, size_t height, string algorithm, bool display=false, 
    bool save=false, string file_path="maze.txt", bool save_binary=false);