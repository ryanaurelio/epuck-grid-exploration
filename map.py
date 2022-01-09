from sketch import *
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
            self.height = height
            self.width = width
            self.movement = []
            self.old_coordinate = (0, 0)

            # Create a grid with border
            self.grid.append(["x"] * width)
            for _ in range(height-2):
                self.grid.append(["x"] + ["."]*(width-2) + ["x"])
            self.grid.append(["x"] * width)
        else:
            # With string
            # Initialize
            rows = string.split("/")
            self.grid = [list(row) for row in rows]
            self.height = len(self.grid)
            self.width = len(self.grid[0])
            self.movement = []
            self.old_coordinate = (0, 0)

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

    # bfs
    # def fill(self, row, column, new_symbol):
    #     old_symbol = self.grid[row][column]
    #     if old_symbol == new_symbol:
    #         return
    #     q = deque()
    #     q.append((row, column))
    #     while q:
    #         i, j = q.pop()
    #         if i > self.width or i < 0 or j > self.height or j < 0 or self.grid[i][j] == 'x' or self.grid[i][j] == 'o':
    #             continue
    #         else:
    #             self.grid[i][j] = new_symbol
    #             q.append((i, j + 1))
    #             q.append((i, j - 1))
    #             q.append((i + 1, j))
    #             q.append((i - 1, j))

    def dfs(self, new_row, new_column, old_row, old_column, new_symbol):
        if new_row > self.width or new_row < 0 or new_column > self.height or new_column < 0 or \
                self.grid[new_row][new_column] == 'x' or self.grid[new_row][new_column] == 'o':
            return
        else:
            self.move_far(new_row, new_column, old_row, old_column)
            self.grid[old_row][old_column] = new_symbol
            self.grid[new_row][new_column] = 'e'
            self.dfs(new_row + 1, new_column, new_row, new_column, new_symbol)
            self.dfs(new_row - 1, new_column, new_row, new_column, new_symbol)
            self.dfs(new_row, new_column + 1, new_row, new_column, new_symbol)
            self.dfs(new_row, new_column - 1, new_row, new_column, new_symbol)

            if self.grid[new_row][new_column] == 'e':
                self.old_coordinate = (new_row, new_column)


    def fill(self, i, j):
        if self.grid[i][j] == 'x' or self.grid[i][j] == 'o':
            return
        self.dfs(i, j, i, j, 'o')

    def write_movement(self, new_move):
        self.movement.append(new_move)

    def move(self, move_forward, turn):
        while (move_forward):
            if move_forward > 0:
                command_move = 'S'
                move_forward -= 1
                self.write_movement(command_move)
            elif move_forward < 0:
                command_move = 'W'
                move_forward += 1
                self.write_movement(command_move)
        while (turn):
            if turn > 0:
                command_move = "D"
                turn -= 1
                self.write_movement(command_move)
            elif turn < 0:
                command_move = "A"
                turn += 1
                self.write_movement(command_move)


    def move_far(self, new_row, new_column, old_row, old_column):
        # case robot stuck
        i, j = self.old_coordinate
        if self.grid[i][j] == 'e':
            self.grid[i][j] = 'o'
            move_forward = new_row - i
            turn = new_column - j
            self.move(move_forward, turn)
        else:
            # anything else
            move_forward = new_row - old_row
            turn = new_column - old_column
            self.move(move_forward, turn)



# m = Map(10, 10)
# m.set_grid(4, 6, 'x')
# m.set_grid(4, 7, 'x')
# m.set_grid(4, 8, 'x')
# m.set_grid(8, 1, "e")
# m.fill(8, 1)

m = Map(6, 6)
m.set_grid(2, 1, 'x')
m.set_grid(2, 2, 'x')
m.set_grid(3, 1, 'e')
m.fill(3, 1)
# m = Map(10, 10)
# m.set_grid(3, 4, "o")
# print(m)
# m = Map(string = "xxxxxxxxxx/x........x/x........x/x...o....x/x........x/x........x/x........x/x........x/x........x/xxxxxxxxxx")
print(m)
print(''.join(m.movement))
print(m.old_coordinate)
print(m.movement)
move_sequence(''.join(m.movement))

ser.close()
