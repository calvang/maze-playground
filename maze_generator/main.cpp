#include "maze.h"

int main() {
    string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    for (string alg : algorithms)
        benchmark_maze(25, 25, alg, true, true, "maze_examples/" + alg + "_maze.txt", false);
    for (string alg : algorithms)
        benchmark_maze(25, 25, alg, true, true, "maze_examples/" + alg + "_maze_binary.txt", true);
    // auto i = recursive_division(10, 5);
}