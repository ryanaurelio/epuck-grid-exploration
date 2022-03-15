from map import *

def flood_fill(m: Map, row: int, column: int, reachable: list) -> None:
    """
    Flood fill algorithm to search reachable grids as tuples of (row, column) and put it in the given list
    :param m: Map that is going to be filled
    :param row: Starting row
    :param column: Starting column
    :param reachable: List of reachable grids
    """
    loc = m.grid[row][column]
    if loc == 'x' or (row, column) in reachable:
        return
    reachable.append((row, column))
    flood_fill(m, row + 1, column, reachable)   # South
    flood_fill(m, row - 1, column, reachable)   # North
    flood_fill(m, row, column - 1, reachable)   # West
    flood_fill(m, row, column + 1, reachable)   # East

def get_unexplored_grids(m: Map, row: int, column: int) -> list:
    """
    Returns a list of unexplored grids in the current map starting from certain location
    :param m: Map to be explored
    :param row: Starting row
    :param column: Starting column
    :return: List of unexplored grids as tuple (row, column)
    """
    reachable = []
    flood_fill(m, row, column, reachable)
    return [(r, c) for r, c in reachable if m.grid[r][c] == '.']

def check_valid_grid(m: Map, explored: set, direction: tuple) -> bool:
    """
    Check whether a movement to a certain grid is valid
    :param m: Map to be explored
    :param explored: Set of explored areas
    :param direction: Direction, where the robot will go
    :return:
    """
    grid_symbol = m.grid[direction[0]][direction[1]]
    return direction not in explored and (grid_symbol == 'o' or grid_symbol == '.')

def get_nearest_unexplored(m: Map, unexplored_grids: list, row: int, column: int) -> list:
    """
    Find a path using BFS to get the nearest unexplored grid. Returns a sequence of grids as path
    :param m: Map to be explored
    :param unexplored_grids: List of unexplored grids
    :param row: Starting row
    :param column: Starting column
    :return: Path to the nearest unexplored grid as a list of tuple (row, column)
    """
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

def parse_path(path: list) -> str:
    """
    Parse a path to a sequence of WASD movement
    :param path: list of coordinates (row, column) as the movement of the robot
    :return: Sequence of WAD movement for the current path
    """
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

def explore(m: Map, row: int, column: int) -> str:
    """
    Returns a sequence of WASD movement to fully explore the reachable grids
    :param m: Map to be explored
    :param row: Starting row
    :param column: Starting column
    :return: Sequence of WASD movement to fully explore the map
    """
    seq = ""
    m.set_grid(row, column, 'o')
    unexplored_grids = get_unexplored_grids(m, row, column)
    while unexplored_grids:
        path = get_nearest_unexplored(m, unexplored_grids, row, column)
        row, column = path[-1]
        seq += parse_path(path)
        m.set_grid(row, column, 'o')
        unexplored_grids.remove(path[-1])
    return seq
