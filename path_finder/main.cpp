#include <iostream>
#include "../maze_generator/maze.h"
#include "path.h"

using std::cout;

void test_random_small() {
    string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    for (string alg : algorithms) {
        auto maze = benchmark_maze(5, 5, alg, true, true, "maze_examples/" + alg + "_maze_small_binary.txt", true);
        auto start = random_coordinate(5, 5);
        auto end = random_coordinate(5, 5);
        a_star(*maze, start.first, start.second, end.first, end.second);
        display_path(*maze, true, true);
        save_path(*maze, "path_examples/" + alg + "_path_small.txt");
    }
}

void test_random_large() {
    string algorithms[] = { "dfs", "kruskal", "prim", "aldous-broder" };
    for (string alg : algorithms) {
        auto maze = benchmark_maze(25, 25, alg, true, true, "maze_examples/" + alg + "_maze_binary.txt", true);
        auto start = random_coordinate(25, 25);
        auto end = random_coordinate(25, 25);
        a_star(*maze, start.first, start.second, end.first, end.second);
        display_path(*maze, true, true);
        save_path(*maze, "path_examples/" + alg + "_path.txt");
    }
}

void test_path_small() {
    string small_file = "maze_examples/aldous-broder_maze_small.txt";
    auto small_maze = load_maze(small_file);
    display_maze(*small_maze);
    a_star(*small_maze, 0, 1, 3, 4);
    display_path(*small_maze, true, true);

    string small_bin_file = "maze_examples/aldous-broder_maze_small_binary.txt";
    auto small_bin_maze = load_maze(small_bin_file);
    display_maze(*small_bin_maze);
    a_star(*small_bin_maze, 0, 0, 4, 4);
    display_path(*small_bin_maze, true, true);
}

void test_path_large() {
    string file = "maze_examples/aldous-broder_maze.txt";
    auto maze = load_maze(file);
    a_star(*maze, 0, 0, 24, 24);
    display_path(*maze, true, true);
    
    string bin_file = "maze_examples/aldous-broder_maze_binary.txt";
    auto bin_maze = load_maze(bin_file);
    a_star(*bin_maze, 24, 0, 0, 24);
    display_path(*bin_maze, true, true);
}

void test_load_path() {
    auto maze = load_path("path_examples/aldous-broder_path.txt");
    display_path(*maze, true, true);
}

int main() {
    // test_small();
    test_random_small();
    test_random_large();
    // test_load_path();
}