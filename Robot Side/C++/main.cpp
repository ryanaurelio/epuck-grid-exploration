#include <iostream>
#include <string>

#include "map.h"
#include "pathfinder.h"

int main() {
    Map map(6, 6);
    map.setObstacle(3, 2);
    map.setObstacle(3, 3);
    map.setObstacle(3, 4);
    map.printMap();

    string seq = explore(map, 4, 1);
    cout << seq;

    return 0;
}
