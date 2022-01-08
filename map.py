class Map:

    # Available symbols
    symbols = [
        "x",    # Obstacle / Wall
        ".",    # Unexplored
        "o",    # Explored
        "e",    # e-puck robot
    ]

    def __init__(self, height, width):
        """Initialize a map with border"""
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

    def __repr__(self):
        """Represents map as string"""
        repr = ""
        for row in self.grid:
            repr += "".join(row) + "\n"
        return repr

    def set_grid(self, row, column, symbol):
        """Set a status for a specific grid"""
        if row == 0 or row == self.width-1 or column == 0 or column == self.height-1:
            raise ValueError("Can't modify map's border.")
        if symbol not in self.symbols:
            raise ValueError("Unknown symbol.\nx = Obstacle / Wall\n. = Unexplored\no = Explored\ne = e-puck robot\n")
        self.grid[row][column] = symbol

m = Map(10, 10)
m.set_grid(3, 4, "o")
print(m)
