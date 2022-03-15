#include <tuple>
#include <algorithm>
#include <string>
#include <stdexcept>
#include "map.h"
#include "pathfinder.h"

void floodFill(Map map, int row, int column, tupleList& reachable) {
    intTuple loc(row, column);
    char symbol = map.m_grid[row][column];
    if (symbol == 'x' || find(reachable.begin(), reachable.end(), loc) != reachable.end())
        return;
    reachable.push_back(loc);
    floodFill(map, row+1, column, reachable);    // South
    floodFill(map, row-1, column, reachable);    // North
    floodFill(map, row, column-1, reachable);    // West
    floodFill(map, row, column+1, reachable);    // East
}

tupleList getUnexploredGrids(const Map& map, int row, int column) {
    tupleList reachable;
    floodFill(map, row, column, reachable);

    tupleList unexplored;
    copy_if(reachable.begin(), reachable.end(), back_inserter(unexplored), [map](intTuple loc){
        return map.m_grid[get<0>(loc)][get<1>(loc)] == '.';
    });
    
    return unexplored;
}

bool checkValidGrid(Map map, tupleList explored, intTuple direction) {
    char gridSymbol = map.m_grid[get<0>(direction)][get<1>(direction)];
    bool inExplored = find(explored.begin(), explored.end(), direction) != explored.end();
    bool validSymbol = gridSymbol == 'o' || gridSymbol == '.';
    return !inExplored && validSymbol;
}

tupleList getNearestUnexplored(const Map& map, tupleList unexploredGrids, int row, int column) {
    tupleListList paths{tupleList {intTuple(row,column)}};
    tupleList explored{intTuple(row, column)};

    while (!unexploredGrids.empty()) {
        tupleListList newPaths;
        for (auto p : paths) {
            int currRow, currCol;
            tie(currRow, currCol) = p.back();

            intTuple south(currRow+1, currCol);
            intTuple north(currRow-1, currCol);
            intTuple west(currRow, currCol-1);
            intTuple east(currRow, currCol+1);

            if (find(unexploredGrids.begin(), unexploredGrids.end(), south) != unexploredGrids.end()) {
                p.push_back(south);
                return p;
            }
            if (find(unexploredGrids.begin(), unexploredGrids.end(), north) != unexploredGrids.end()) {
                p.push_back(north);
                return p;
            }
            if (find(unexploredGrids.begin(), unexploredGrids.end(), west) != unexploredGrids.end()) {
                p.push_back(west);
                return p;
            }
            if (find(unexploredGrids.begin(), unexploredGrids.end(), east) != unexploredGrids.end()) {
                p.push_back(east);
                return p;
            }
            
            if (checkValidGrid(map, explored, south)) {
                explored.push_back(south);
                tupleList newP = p;
                newP.push_back(south);
                newPaths.push_back(newP);
            }
            if (checkValidGrid(map, explored, north)) {
                explored.push_back(north);
                tupleList newP = p;
                newP.push_back(north);
                newPaths.push_back(newP);
            }
            if (checkValidGrid(map, explored, west)) {
                explored.push_back(west);
                tupleList newP = p;
                newP.push_back(west);
                newPaths.push_back(newP);
            }
            if (checkValidGrid(map, explored, east)) {
                explored.push_back(east);
                tupleList newP = p;
                newP.push_back(east);
                newPaths.push_back(newP);
            }
        }
        paths = newPaths;
    }
    return tupleList{};
}

string parsePath(tupleList path) {
    string seq;
    for (int i = 0; i < path.size()-1; i++) {
        int pr, pc, qr, qc, r, c;
        tie(pr, pc) = path[i];
        tie(qr, qc) = path[i+1];
        r = qr - pr;
        c = qc - pc;
        if (r == -1)
            seq.push_back('W');
        else if (c == -1)
            seq.push_back('A');
        else if (r == 1)
            seq.push_back('S');
        else if (c == 1)
            seq.push_back('D');
        else
            throw invalid_argument("Invalid movement.");
    }
    return seq;
}

string explore(Map map, int row, int column) {
    string seq;
    map.setExplored(row, column);
    tupleList unexploredGrids = getUnexploredGrids(map, row, column);
    while (!unexploredGrids.empty()) {
        tupleList path = getNearestUnexplored(map, unexploredGrids, row, column);
        tie(row, column) = path.back();
        seq += parsePath(path);
        map.setExplored(row, column);
        unexploredGrids.erase(remove(
                unexploredGrids.begin(),
                unexploredGrids.end(),
                path.back()
            ), unexploredGrids.end());
    }
    return seq;
}
