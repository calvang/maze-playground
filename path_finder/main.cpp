#include <iostream>
#include "../maze_generator/maze.h"
#include "path.h"

using std::cout;

int main() {
    // string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    // for (string alg : algorithms)
    //     benchmark_maze(50, 25, alg, true, true, "maze_examples/" + alg + "_maze.txt", false);
    // for (string alg : algorithms)
    //     benchmark_maze(50, 25, alg, true, true, "maze_examples/" + alg + "_maze_binary.txt", true);
    string file = "maze_examples/aldous-broder_maze.txt";
    auto maze = open_maze(file);
    display_maze(*maze);
    
    string bin_file = "maze_examples/aldous-broder_maze_binary.txt";
    auto bin_maze = open_maze(bin_file);
    display_maze(*bin_maze);
}