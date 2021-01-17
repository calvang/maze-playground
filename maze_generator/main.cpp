#include "maze.h"

int main() {
    string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    for (string alg : algorithms)
        benchmark_maze(50, 25, alg, true);
}