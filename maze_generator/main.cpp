#include "maze.h"

void test_small() {
    string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    for (string alg : algorithms)
        benchmark_maze(5, 5, alg, true, true, "maze_examples/" + alg + "_maze_small.txt", false);
    for (string alg : algorithms)
        benchmark_maze(5, 5, alg, true, true, "maze_examples/" + alg + "_maze_small_binary.txt", true);
}

void test_large() {
    string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    for (string alg : algorithms)
        benchmark_maze(25, 25, alg, true, false, "maze_examples/" + alg + "_maze.txt", false);
    for (string alg : algorithms)
        benchmark_maze(25, 25, alg, true, true, "maze_examples/" + alg + "_maze_binary.txt", true);
}

int main() {
    test_small();
    test_large();
    // auto i = recursive_division(10, 5);
}