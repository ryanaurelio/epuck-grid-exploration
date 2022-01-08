class Map:
    def __init__(self, height, width):
        if height < 3 or width < 3:
            raise ValueError("Minimum size 3x3.")
        self.grid = []
        self.grid.append(["x"] * width)
        for _ in range(height-2):
            self.grid.append(["x"] + ["."]*(width-2) + ["x"])
        self.grid.append(["x"] * width)

    def __repr__(self):
        repr = ""
        for row in self.grid:
            repr += "".join(row) + "\n"
        return repr

    def set_grid(self, status):
        pass

m = Map(10, 10)
print(m)
