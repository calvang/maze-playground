#include <string>
#include <vector>
#include <memory>

using std::unique_ptr;
using std::string;
using std::vector;


void display_path(vector<vector<int>>& grid, bool colors=false, bool visited=false);

void save_path(vector<vector<int>>& grid, string file_path);

unique_ptr<vector<vector<int>>> load_path(string file_path);

bool a_star(vector<vector<int>>& grid, size_t startX, size_t startY, size_t endX, size_t endY,
    string heuristic="manhattan", bool track_visited=true);