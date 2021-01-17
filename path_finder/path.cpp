#include <iostream>
#include "path.h"

using std::cout;

/**
 * Print maze and path to stdout
 */ 
void display_path(vector<vector<int>>& grid) {
    for (size_t j = 0; j < grid[0].size(); ++j) {
        for (size_t i = 0; i < grid.size(); ++i) {
            if (grid[i][j] == 1)
                cout << "█";
            else if (grid[i][j] == 0)
                cout << " ";
            else 
                cout << "+";
        }
        cout << "\n";
    }
}