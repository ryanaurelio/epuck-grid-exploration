#ifndef MAP_H
#define MAP_H

#include <vector>
using namespace std;

class Map {
public:
    vector<vector<char>> m_grid;
    int m_height;
    int m_width;

    Map(int height, int width);
    void printMap();
    void setObstacle(int x, int y);
    void setUnexplored(int x, int y);
    void setExplored(int x, int y);
    void setRobot(int x, int y);
};

#endif
