from collections import deque
from copy import copy, deepcopy

import numpy as np
class Map:
    """Map class"""
    # Available symbols
    symbols = [
        "x",  # Obstacle / Wall
        ".",  # Unexplored
        "o",  # Explored
        "e",  # e-puck robot
    ]

    def __init__(self, height=None, width=None, string=None):
        """Initialize a map with border using height x width or string with "/" as new line."""
        # Arguments check
        if (height is None or width is None) and string is None:
            raise Exception("Invalid arguments. Please provide height and width or map string.")

        if string is None:
            # With height and width
            # Bound check
            if height < 3 or width < 3:
                raise ValueError("Minimum size 3x3.")

            # Initialize
            self.grid = []
            self.home_grid = []
            self.height = height
            self.width = width
            self.robots = []
            self.movement = []
            self.old_coordinate = None
            self.home_coordinate = (0, 0)
            self.status = 'IDLE'
            self.path = []

            # Create a grid with border
            self.grid.append(["x"] * width)
            for _ in range(height-2):
                self.grid.append(["x"] + ["."]*(width-2) + ["x"])
            self.grid.append(["x"] * width)

            self.home_grid.append(['x']* width)
            for _ in range(height - 2):
                self.home_grid.append(["x"] + [0] * (width - 2) + ["x"])
            self.home_grid.append(["x"] * width)

        else:
            # With string
            # Initialize
            rows = string.split("/")
            self.grid = [list(row) for row in rows]
            self.height = len(self.grid)
            self.width = len(self.grid[0])
            self.movement = []
            self.old_coordinate = (0, 0)
            self.home_grid = []
            self.home_coordinate = (0, 0)
            self.status = 'IDLE'

            # Shape check
            if not all(len(row) == self.width for row in self.grid):
                raise Exception("Invalid shape.")
            for i in range(self.height):
                for j in range(self.width):
                    if self.grid[i][j] not in self.symbols:
                        # Border check
                        if (i == 0 or i == self.height-1 or j == 0 or j == self.width-1) and self.grid[i][j] != "x":
                            raise Exception("Invalid border")
                        # Unknown symbols check
                        raise ValueError(
                            "Unknown symbol.\nx = Obstacle / Wall\n. = Unexplored\no = Explored\ne = e-puck robot\n")

    def __repr__(self):
        """Represents map as string"""
        rep = ""
        for row in self.grid:
            rep += "".join(row) + "\n"
        return rep

    def set_grid(self, row, column, symbol):
        """Set a status for a specific grid"""
        if row == 0 or row == self.width - 1 or column == 0 or column == self.height - 1:
            raise ValueError("Can't modify map's border.")
        if symbol not in self.symbols:
            raise ValueError("Unknown symbol.\nx = Obstacle / Wall\n. = Unexplored\no = Explored\ne = e-puck robot\n")
        self.grid[row][column] = symbol
        if symbol == 'x':
            self.home_grid[row][column] = symbol
        else:
            self.home_grid[row][column] = 0

    # bfs
    # def find_new_path(self, row, column):
    #     # old_symbol = self.grid[row][column]
    #     # if old_symbol == new_symbol:
    #     #     return
    #     path_already_o = []
    #     path = []
    #     q = deque()
    #     q.append((row, column))
    #     while q:
    #         i, j = q.pop()
    #         if i > self.width or i < 0 or j > self.height or j < 0 or self.grid[i][j] == 'x':
    #             continue
    #         elif self.grid[i][j] == 'o':
    #             self.grid[i][j] ='x'
    #             path_already_o.append((i,j))
    #             path.append((i,j))
    #             q.append((i, j + 1))
    #             q.append((i, j - 1))
    #             q.append((i + 1, j))
    #             q.append((i - 1, j))
    #         elif self.grid[i][j] == '.':
    #             path.append((i,j))
    #             return path
    #         else:
    #             # self.grid[i][j] = new_symbol
    #             path.append((i,j))
    #             q.append((i, j + 1))
    #             q.append((i, j - 1))
    #             q.append((i + 1, j))
    #             q.append((i - 1, j))
    #

    def discard_path(self, path, home_row, home_column):
        discarded_path = []
        path.reverse()
        print(path)
        for coordinate in path:
            last_row, last_column = coordinate
            if (((last_row + 1 == home_row or last_row - 1 == home_row or last_row == home_row) and last_column == home_column) or
                ((last_column + 1 == home_column or last_column - 1 == home_column or last_column == home_column) and last_row == home_row)):
                home_row, home_column = coordinate
                continue
            discarded_path.append(coordinate)
        print(discarded_path)
        for coordinate in discarded_path:
            path.remove(coordinate)
        path.reverse()
        print(path)
        return path

    def find_new_path(self, new_row, new_column, path = None, map = None):
        if path is None:
            path = []
        if map is None:
            map = deepcopy(self.grid)
        """ find new path algorithm with dfs"""

        """
        checks:
        1. if the path is finished
        2. if it is a new path
        3. if overflow in height or width
        4. if it is already passed by another epuck
        """
        if self.path:
            return
        if self.grid[new_row][new_column] == '.':
            path.append((new_row, new_column))
            home_row, home_column = path[-1]
            path = self.discard_path(path, home_row, home_column)
            self.path = path
            self.grid = map
            return
        if new_row > self.height or new_row < 0 or new_column > self.width or new_column < 0 or \
                self.grid[new_row][new_column] == 'x' or (new_row, new_column) in path:
            return
        else:
            if self.grid[new_row][new_column] == 'o':
                self.grid[new_row][new_column] = 'x'

            """
            dfs algorithm
            """
            path.append((new_row, new_column))
            newthing = []
            newthing.append((self.grid[new_row + 1][new_column], new_row + 1, new_column))
            newthing.append((self.grid[new_row - 1][new_column], new_row - 1, new_column))
            newthing.append((self.grid[new_row][new_column + 1], new_row, new_column + 1))
            newthing.append((self.grid[new_row][new_column - 1], new_row, new_column - 1))
            newthing.sort()
            for thing in newthing:
                sign, row, col = thing
                self.find_new_path(row, col, path, map)


    def find_home(self, new_row, new_column, old_row = None, old_column = None, path = None, map = None):
        if path is None:
            path = []
        if map is None:
            map = deepcopy(self.grid)
        if old_row is None:
            old_row = new_row
        if old_column is None:
            old_column = new_column
        """ find new path algorithm with dfs"""
        """
        checks:
        1. if the path is finished
        2. if it is a new path
        3. if overflow in height or width
        4. if it is already passed by another epuck
        """
        if self.path:
            return

        home_row, home_column = self.home_coordinate
        if new_row == home_row and new_column == home_column:
            path = self.discard_path(path, home_row, home_column)
            path.append((new_row, new_column))
            self.path = path
            self.grid = map
            return

        if new_row > self.height or new_row < 0 or new_column > self.width or new_column < 0 or \
                self.grid[new_row][new_column] == 'x' or (new_row, new_column) in path:
            return
        else:
            if self.grid[new_row][new_column] == 'o' or self.grid[new_row][new_column] == '.':
                self.grid[new_row][new_column] = 'x'
            """
            dfs algorithm
            """
            path.append((new_row, new_column))
            self.find_home(new_row + 1, new_column, new_row, new_column, path, map)
            self.find_home(new_row - 1, new_column, new_row, new_column, path, map)
            self.find_home(new_row, new_column + 1, new_row, new_column, path, map)
            self.find_home(new_row, new_column - 1, new_row, new_column, path, map)


    def check_map(self):
        """
        check if the map has already been fully explored
        Returns: none

        """
        for i in range(self.height):
            for j in range(self.width):
                if self.grid[i][j] == '.':
                    return False
        return True


    def dfs(self, new_row, new_column, old_row, old_column, new_symbol):
        """ fill algorithm with dfs"""

        """
        checks:
        1. if the row and column is inside map
        2. if the grid can't be passed through ('x')
        3. the grid has already been passed ('o')
        """
        # todo change to something more elegant hehe (self.status bad)
        if self.check_map() and not self.status == 'HOME':
            self.find_home(old_row, old_column)
            print(self.path[1:])
            for path in self.path[1:]:
                row, column = path
                self.grid[old_row][old_column] = new_symbol
                self.grid[row][column] = 'e'
                self.write_movement(row - old_row, column - old_column)
                old_row, old_column = path
            self.path = []
            self.status = "HOME"
            return

        if self.old_coordinate:
            a, b = self.old_coordinate
            if a != new_row or b != new_column:
                return
            self.old_coordinate = None

        if new_row > self.height or new_row < 0 or new_column > self.width or new_column < 0 or \
                self.grid[new_row][new_column] == 'x' or self.grid[new_row][new_column] == 'o' or self.status == 'HOME':
            return
        else:
            """
            move one block according to difference of the x and y axis
            change the symbol from the e-puck
            add value to the home_grid (for the backtrack)
            """
            self.move(new_row, new_column, old_row, old_column)
            self.grid[old_row][old_column] = new_symbol
            self.grid[new_row][new_column] = 'e'

            self.path = []
            """
            dfs algorithm
            """
            self.dfs(new_row + 1, new_column, new_row, new_column, new_symbol)
            self.dfs(new_row - 1, new_column, new_row, new_column, new_symbol)
            self.dfs(new_row, new_column + 1, new_row, new_column, new_symbol)
            self.dfs(new_row, new_column - 1, new_row, new_column, new_symbol)

            # in case the epuck is stuck.
        # todo change to something more elegant hehe (self.path here bad)
            if not self.check_map() and not self.path:
                self.find_new_path(new_row, new_column)
                self.old_coordinate = self.path[-1]
                old_row = new_row
                old_column = new_column
                for path in self.path[:-1]:
                    row, column = path
                    self.grid[old_row][old_column] = new_symbol
                    self.grid[row][column] = 'e'
                    self.write_movement(row - old_row, column - old_column)
                    old_row, old_column = path


    def fill(self, i, j):
        """
        algorithm for filling the map.
        Args:
            i: x
            j: y
            both are useful for getting back to first place.

        Returns: none

        """
        self.home_coordinate = (i, j)
        if self.grid[i][j] == 'x' or self.grid[i][j] == 'o':
            return
        self.dfs(i, j, i, j, 'o')

    def write_movement(self, move_forward, turn):
        """
        for writing the movement that will be done by the robot.
        TODO change the "self.movement.append" to the robot movement if the robot is moving in real time
        Args:
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
        self.movement.append(command_move)

    def move(self, new_row, new_column, old_row, old_column):
        """
        function for calculating the move of the robot.
        Args:
            new_row: row right now
            new_column: column right now
            old_row: row one step before
            old_column: column one step before

        Returns: none

        """
        move_forward = new_row - old_row
        turn = new_column - old_column
        self.write_movement(move_forward, turn)



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

m = Map(10, 10)
m.set_grid(4, 6, 'x')
m.set_grid(4, 7, 'x')
m.set_grid(4, 8, 'x')
m.set_grid(4, 5, 'o')
m.set_grid(4, 4, 'o')
m.set_grid(4, 3, 'o')
m.set_grid(5, 5, 'o')
m.set_grid(5, 3, 'o')
m.set_grid(6, 5, 'o')
m.set_grid(6, 4, 'o')
m.set_grid(6, 3, 'o')
m.set_grid(4, 4, "e")
# m.set_grid(1, 1, "e")
m.fill(4, 4)
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
print(m)
print(''.join(m.movement))
print(m.old_coordinate)
print(m.movement)
# move_sequence(''.join(m.movement))
print(m.home_grid)

# ser.close()
