#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <string>

typedef tuple<int, int> intTuple;
typedef vector<intTuple> tupleList;
typedef vector<tupleList> tupleListList;

void floodFill(Map map, int row, int column, tupleList& reachable);
tupleList getUnexploredGrids(const Map& map, int row, int column);
bool checkValidGrid(Map map, tupleList explored, intTuple direction);
tupleList getNearestUnexplored(const Map& map, tupleList unexploredGrids, int row, int column);
string parsePath(tupleList path);
string explore(Map map, int row, int column);

#endif
