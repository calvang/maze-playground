#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

void display_maze(vector<vector<int>>& grid);

unique_ptr<vector<vector<int>>> randomized_depth_first_search(size_t width, size_t height, 
    size_t startX=0, size_t startY=0, bool show_frames=false);

unique_ptr<vector<vector<int>>> kruskal(size_t width, size_t height, bool show_frames=false);

unique_ptr<vector<vector<int>>> prim(size_t width, size_t height, size_t startX=0, 
    size_t startY=0, bool show_frames=false);