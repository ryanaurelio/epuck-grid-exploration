from map import *

def discard_path(path, home_row, home_column):
    """

    Args:
        path: list of coordinates
        home_row: the last x coordinate
        home_column: the last y coordinate

    Returns: list of coordinates that frees up the discarded coordinates
    Examples:
        [(1,2), (1,3), (4,2), (5,2), (1,4)]
        becomes
        [(1,2), (1,3), (1,4)]
        because (4,2) doesn't connect with (1,3) and therefore we remove (4,2) and (5,2)

    """
    discarded_path = []
    path.reverse()
    for coordinate in path:
        last_row, last_column = coordinate
        if (((last_row + 1 == home_row or last_row - 1 == home_row or last_row == home_row) and last_column == home_column) or
            ((last_column + 1 == home_column or last_column - 1 == home_column or last_column == home_column) and last_row == home_row)):
            home_row, home_column = coordinate
            continue
        discarded_path.append(coordinate)
    for coordinate in discarded_path:
        path.remove(coordinate)
    path.reverse()
    return path

def find_new_path(m: Map, new_row, new_column, path = None, grid = None, home = False):
    """

    Args:
        m: Map
        new_row: x coordinate
        new_column: y coordinate
        path: list of coordinates
        grid: grid of the map (to save the old map)
        home: True if want to return Home, False if want to find new path

    Returns: list of coordinates until a new '.'/ home is found

    """
    if path is None:
        path = []
    if grid is None:
        grid = deepcopy(m.grid)
    """ find new path algorithm with dfs"""

    """
    checks:
    1. if the path is finished
    2. if it is a new path
    3. if overflow in height or width
    4. if it is already passed by another epuck
    """
    if m.path:
        return
    if home:
        home_row, home_column = m.home_coordinate
        if new_row == home_row and new_column == home_column:
            path = discard_path(path, home_row, home_column)
            path.append((new_row, new_column))
            m.path = path
            m.grid = grid
            return
    else:
        if m.grid[new_row][new_column] == '.':
            path.append((new_row, new_column))
            home_row, home_column = path[-1]
            path = discard_path(path, home_row, home_column)
            m.path = path
            m.grid = grid
            return

    if new_row > m.height or new_row < 0 or new_column > m.width or new_column < 0 or \
            m.grid[new_row][new_column] == 'x' or (new_row, new_column) in path:
        return
    else:
        if m.grid[new_row][new_column] == 'o':
            m.grid[new_row][new_column] = 'x'

        """
        dfs algorithm
        """
        path.append((new_row, new_column))
        direction_sign = [(m.grid[new_row + 1][new_column], new_row + 1, new_column),
                          (m.grid[new_row - 1][new_column], new_row - 1, new_column),
                          (m.grid[new_row][new_column + 1], new_row, new_column + 1),
                          (m.grid[new_row][new_column - 1], new_row, new_column - 1)]
        direction_sign.sort()
        for coordinate in direction_sign:
            sign, row, col = coordinate
            find_new_path(m, row, col, path, grid, home)

def check_map(m: Map):
    """
    check if the map has already been fully explored
    Returns: bool

    """
    for i in range(m.height):
        for j in range(m.width):
            if m.grid[i][j] == '.':
                return False
    return True

def write_path_movement(m: Map, paths, old_row, old_column, new_symbol):
    for path in paths:
        row, column = path
        m.grid[old_row][old_column] = new_symbol
        m.grid[row][column] = 'e'
        write_movement(m, row - old_row, column - old_column)
        old_row, old_column = path

def dfs(m: Map, new_row, new_column, old_row, old_column, new_symbol):
    """ fill algorithm with dfs"""

    """
    checks:
    1. if the row and column is inside map
    2. if the grid can't be passed through ('x')
    3. the grid has already been passed ('o')
    """

    if m.old_coordinate:
        if m.old_coordinate == m.home_coordinate:
            return
        a, b = m.old_coordinate
        if a != new_row or b != new_column:
            return
        m.old_coordinate = None

    if check_map(m):
        find_new_path(m, old_row, old_column, home = True)
        write_path_movement(m, m.path[1:], old_row, old_column, new_symbol)
        m.path = []
        m.old_coordinate = m.home_coordinate
        return

    if new_row > m.height or new_row < 0 or new_column > m.width or new_column < 0 or \
            m.grid[new_row][new_column] == 'x' or m.grid[new_row][new_column] == 'o':
        return
    else:
        """
        move one block according to difference of the x and y axis
        change the symbol from the e-puck
        add value to the home_grid (for the backtrack)
        """
        write_movement(m, new_row - old_row, new_column - old_column)
        m.grid[old_row][old_column] = new_symbol
        m.grid[new_row][new_column] = 'e'

        m.path = []
        """
        dfs algorithm
        """
        dfs(m, new_row + 1, new_column, new_row, new_column, new_symbol)
        dfs(m, new_row - 1, new_column, new_row, new_column, new_symbol)
        dfs(m, new_row, new_column + 1, new_row, new_column, new_symbol)
        dfs(m, new_row, new_column - 1, new_row, new_column, new_symbol)

        # in case the epuck is stuck.
        if not check_map(m) and not m.old_coordinate:
            find_new_path(m, new_row, new_column)
            m.old_coordinate = m.path[-1]
            old_row = new_row
            old_column = new_column
            write_path_movement(m, m.path[:-1], old_row, old_column, new_symbol)


def fill(m: Map, i, j):
    """
    algorithm for filling the map.
    Args:
        m: Map
        i: x
        j: y
        both are useful for getting back to first place.

    Returns: none

    """
    m.home_coordinate = (i, j)
    if m.grid[i][j] == 'x' or m.grid[i][j] == 'o':
        return
    dfs(m, i, j, i, j, 'o')

def write_movement(m: Map, move_forward, turn):
    """
    for writing the movement that will be done by the robot.
    TODO change the "self.movement.append" to the robot movement if the robot is moving in real time
    Args:
        m: Map
        move_forward: up = +1 , down = -1
        turn: left = -1, right  = +1

    Returns: none

    """
    command_move =''
    if move_forward > 0:
        command_move = 'S'
        move_forward -= 1
    elif move_forward < 0:
        command_move = 'W'
        move_forward += 1
    if turn > 0:
        command_move = "D"
        turn -= 1
    elif turn < 0:
        command_move = "A"
        turn += 1
    m.movement.append(command_move)




# Map
# m = Map(6, 6)
# m.set_grid(3, 2, 'x')
# m.set_grid(3, 3, 'x')
# m.set_grid(3, 4, 'x')
# m.set_grid(4, 1, 'e')
# print(m)
#
# # Explore
# m.fill(4, 1)

# m = Map(5,10)
# m.set_grid(1, 2, 'x')
# m.set_grid(1, 3, 'x')
# m.set_grid(2, 1, 'e')
# m.fill(2,1)

# m = Map(4,4)
# m.set_grid(1,2, 'x')
# m.set_grid(2,1, 'e')
# print(m)
# m.fill(2,1)

# m = Map(10, 10)
# m.set_grid(4, 6, 'x')
# m.set_grid(4, 7, 'x')
# m.set_grid(4, 8, 'x')
# m.set_grid(4, 5, 'o')
# m.set_grid(4, 4, 'o')
# m.set_grid(4, 3, 'o')
# m.set_grid(5, 5, 'o')
# m.set_grid(5, 3, 'o')
# m.set_grid(6, 5, 'o')
# m.set_grid(6, 4, 'o')
# m.set_grid(6, 3, 'o')
# m.set_grid(4, 4, "e")
# m.set_grid(1, 1, "e")
# m.fill(4, 4)
# m.fill(1, 1)

# m = Map(6, 6)
# m.set_grid(2, 1, 'x')
# m.set_grid(2, 2, 'x')
# m.set_grid(3, 1, 'e')
# m.fill(3, 1)

# m = Map(6, 6)
# m.set_grid(2, 3, 'x')
# m.set_grid(2, 2, 'x')
# m.set_grid(3, 1, 'e')
# m.fill(3, 1)
# m = Map(10, 10)
# m.set_grid(3, 4, "o")
# print(m)
# m = Map(string = "xxxxxxxxxx/x........x/x........x/x...o....x/x........x/x........x/x........x/x........x/x........x/xxxxxxxxxx")
# print(m)
# print(''.join(m.movement))
# print(m.old_coordinate)
# print(m.movement)
# move_sequence(''.join(m.movement))

# ser.close()