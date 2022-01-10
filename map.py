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
            self.old_coordinate = (0, 0)
            self.home_coordinate = (0, 0)
            self.status = 'IDLE'

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
    # def bfs(self, row, column, new_symbol):
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

    def check_map(self):
        """
        check if the map has already been fully explored
        Returns: none

        """
        for i in range(self.height):
            for j in range(self.width):
                if self.grid[i][j] == '.':
                    return True
        return False


    def dfs(self, new_row, new_column, old_row, old_column, new_symbol):
        """ fill algorithm with dfs"""

        """
        checks:
        1. if the row and column is inside map
        2. if the grid can't be passed through ('x')
        3. the grid has already been passed ('o')
        """
        if new_row > self.height or new_row < 0 or new_column > self.width or new_column < 0 or \
                self.grid[new_row][new_column] == 'x' or self.grid[new_row][new_column] == 'o':
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
            self.home_grid[new_row][new_column] = self.home_grid[old_row][old_column] + 1

            """
            dfs algorithm
            """
            self.dfs(new_row + 1, new_column, new_row, new_column, new_symbol)
            self.dfs(new_row - 1, new_column, new_row, new_column, new_symbol)
            self.dfs(new_row, new_column + 1, new_row, new_column, new_symbol)
            self.dfs(new_row, new_column - 1, new_row, new_column, new_symbol)

            """
            changing the old coordinate (useful for backtrack)
            check if the map is already been thoroughly explored
            if ja, get back to home (first place)
            """
            self.old_coordinate = (new_row, new_column)
            if self.check_map():
                # if the e-puck comes here but the map isnt yet full, the robot is stuck. We do the backtrack here
                self.status = 'STUCK'
                self.move(new_row, new_column, old_row, old_column)
                self.status = 'WORKING'
            else:
                # map is done, time to go home
                self.move_home()
                self.old_coordinate = self.home_coordinate
                return

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


    def find_another_way(self):
        self.move_home()

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
        # case robot stuck
        if self.status == 'STUCK':
            temp = self.home_coordinate
            i, j = self.old_coordinate
            self.home_coordinate = (old_row, old_column)
            self.find_another_way()
            self.grid[i][j] = 'o'
            self.home_coordinate = temp
        else:
            # anything else
            move_forward = new_row - old_row
            turn = new_column - old_column
            self.write_movement(move_forward, turn)

    def move_home(self):
        """
        algorithm for backtracking movement of the robot.
        TODO highly inefficient, change to another if there's time
        Returns: none

        """

        i, j = self.old_coordinate
        home_row, home_column = self.home_coordinate
        """
        without changing the value of the home_grid[home_row][home_column], sometimes the program will loop indefinitely
        """
        temp = self.home_grid[home_row][home_column]
        self.home_grid[home_row][home_column] = 0
        self.grid[home_row][home_column] = 'e'

        while i != home_row or j != home_column:
            val = [self.home_grid[i + 1][j],
                   self.home_grid[i - 1][j],
                   self.home_grid[i][j + 1],
                   self.home_grid[i][j - 1]
                   ]
            val = [1000000 if e =='x' else e for e in val]

            index = val.index(min(val)) + 1
            direction = ''
            match index:
                case 1:
                    direction = 'S'
                    self.grid[i][j] = 'o'
                    i += 1
                    self.grid[i][j] = 'e'
                case 2:
                    direction = 'W'
                    self.grid[i][j] = 'o'
                    i -= 1
                    self.grid[i][j] = 'e'
                case 3:
                    direction = 'D'
                    self.grid[i][j] = 'o'
                    j += 1
                    self.grid[i][j] = 'e'
                case 4:
                    direction = 'A'
                    self.grid[i][j] = 'o'
                    j -= 1
                    self.grid[i][j] = 'e'

            self.movement.append(direction)
        self.home_grid[home_row][home_column] = temp
        self.status = 'FINISHED'



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
m.set_grid(8, 1, "e")
m.set_grid(1, 1, "e")
m.fill(8, 1)
m.fill(1, 1)

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
