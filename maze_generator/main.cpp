#include <iostream>
#include "maze.h"

int main() {
    auto dfs_maze = randomized_depth_first_search(50, 25);
    display_maze(*dfs_maze);
    auto kruskal_maze = kruskal(50, 25);
    display_maze(*kruskal_maze);
    auto prim_maze = prim(50,25);
    display_maze(*prim_maze);
}