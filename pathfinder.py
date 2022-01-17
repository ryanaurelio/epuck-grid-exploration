from map import *

def flood_fill(m: Map, row, column, reachable):
    """Flood fill algorithm to search reachable nodes."""
    loc = m.grid[row][column]
    if loc == 'x' or (row, column) in reachable:
        return
    reachable.append((row, column))
    flood_fill(m, row + 1, column, reachable)   # South
    flood_fill(m, row - 1, column, reachable)   # North
    flood_fill(m, row, column - 1, reachable)   # West
    flood_fill(m, row, column + 1, reachable)   # East

def get_unexplored_nodes(m: Map, row, column):
    """Returns a list of reachable nodes from certain location."""
    reachable = []
    flood_fill(m, row, column, reachable)
    return [(r, c) for r, c in reachable if m.grid[r][c] == '.']

def explore(m:Map, row, column):
    """Returns a sequence of WASD movement."""
    seq = ""
    m.set_grid(row, column, 'o')
    unexplored_grids = get_unexplored_nodes(m, row, column)
    while unexplored_grids:
        path = get_nearest_unexplored(m, unexplored_grids, row, column)
        row, column = path[-1]
        seq += parse_path(path)
        m.set_grid(row, column, 'o')
        unexplored_grids.remove(path[-1])
    return seq

def parse_path(path):
    seq = ""
    movement = list(zip(path, path[1:]))
    for (pr, pc), (qr, qc) in movement:
        (r, c) = (qr - pr, qc - pc)
        if r == -1:
            seq += 'W'
        elif c == -1:
            seq += 'A'
        elif r == 1:
            seq += 'S'
        elif c == 1:
            seq += 'D'
        else:
            raise Exception("Invalid movement.")
    return seq

def get_nearest_unexplored(m:Map, unexplored_grids, row, column):
    """Find a path using BFS to get the nearest unexplored node. Returns a sequence of node."""
    paths = [[(row, column)]]
    explored = {(row, column)}

    while unexplored_grids:
        new_paths = []
        for i, p in enumerate(paths):
            curr_row, curr_col = p[-1]

            south = (curr_row + 1, curr_col)
            north = (curr_row - 1, curr_col)
            west = (curr_row, curr_col - 1)
            east = (curr_row, curr_col + 1)

            if south in unexplored_grids:
                return p + [south]
            if north in unexplored_grids:
                return p + [north]
            if west in unexplored_grids:
                return p + [west]
            if east in unexplored_grids:
                return p + [east]

            if check_valid_grid(m, explored, south):
                explored.add(south)
                new_paths.append(p + [south])
            if check_valid_grid(m, explored, north):
                explored.add(north)
                new_paths.append(p + [north])
            if check_valid_grid(m, explored, west):
                explored.add(west)
                new_paths.append(p + [west])
            if check_valid_grid(m, explored, east):
                explored.add(east)
                new_paths.append(p + [east])
        paths = new_paths
    return []

def check_valid_grid(m:Map, explored, direction):
    grid_symbol = m.grid[direction[0]][direction[1]]
    return direction not in explored and (grid_symbol == 'o' or grid_symbol == '.')
