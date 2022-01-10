from map import *

def flood_fill(grid: Map, row, column, reachable):
    """Flood fill algorithm to search reachable nodes."""
    loc = grid.grid[row][column]
    if loc == 'x' or (row, column) in reachable:
        return
    reachable.append((row, column))
    flood_fill(grid, row-1, column, reachable)   # South
    flood_fill(grid, row+1, column, reachable)   # North
    flood_fill(grid, row, column-1, reachable)   # West
    flood_fill(grid, row, column+1, reachable)   # East

def reachable_nodes(grid: Map, row, column):
    """Returns a list of reachable nodes from certain location."""
    reachable = []
    flood_fill(grid, row, column, reachable)
    return reachable

def explore(grid:Map, row, column):
    """Returns a sequence of WASD movement."""
    # TODO: Implement
    unexplored = reachable_nodes(grid, row, column)
    unexplored.remove((row, column))
    while len(unexplored) > 0:

def get_nearest_unexplored(grid:Map, unexplored, row, column):
    """Find a path using BFS to get the nearest unexplored node. Returns a sequence of node."""
    # TODO: Implement
    paths = [((row, column),)]
    explored = [(row, column)]
    for i, p in enumerate(paths):
        curr_row, curr_col = p[-1]
        if south in unexplored:
            return p + south
        if south not in explored:
            explored += south
            paths[i] = p


g = Map(5, 5)
g.set_grid(1, 2, 'x')
g.set_grid(2, 3, 'x')
print(g)

print(reachable_nodes(g, 3, 2))
