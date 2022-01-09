class Map:
    """Map class"""
    # Available symbols
    symbols = [
        "x",    # Obstacle / Wall
        ".",    # Unexplored
        "o",    # Explored
        "e",    # e-puck robot
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
        if row == 0 or row == self.width-1 or column == 0 or column == self.height-1:
            raise ValueError("Can't modify map's border.")
        if symbol not in self.symbols:
            raise ValueError("Unknown symbol.\nx = Obstacle / Wall\n. = Unexplored\no = Explored\ne = e-puck robot\n")
        self.grid[row][column] = symbol

# m = Map(10, 10)
# m.set_grid(3, 4, "o")
# print(m)
m = Map(string = "xxxxxxxxxx/x........x/x........x/x...o....x/x........x/x........x/x........x/x........x/x........x/xxxxxxxxxx")
print(m)
