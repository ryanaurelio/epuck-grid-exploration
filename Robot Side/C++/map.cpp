#include <iostream>
#include <stdexcept>
#include <vector>

#include "map.h"

Map::Map(int height, int width) {
    if (height < 3 || width < 3)
        throw invalid_argument("Minimum size is 3x3.");

    m_height = height;
    m_width = width;

    vector<vector<char>> grid(height, vector<char> (width, '.'));
    for (int i = 0; i < m_width; i++)
        grid[0][i] = 'x';
    for (int i = 0; i < m_width; i++)
        grid[m_height-1][i] = 'x';
    for (int i = 0; i < m_height; i++)
        grid[i][0] = 'x';
    for (int i = 0; i < m_height; i++)
        grid[i][m_width-1] = 'x';
    m_grid = grid;
}

void Map::printMap() {
    for (auto & i : m_grid) {
        for (char j : i)
            cout << j << " ";
        cout << endl;
    }
}

void Map::setObstacle(int x, int y) {
    m_grid[x][y] = 'x';
}

void Map::setUnexplored(int x, int y) {
    m_grid[x][y] = '.';
}

void Map::setExplored(int x, int y) {
    m_grid[x][y] = 'o';
}

void Map::setRobot(int x, int y) {
    m_grid[x][y] = 'e';
}
