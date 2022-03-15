from contextlib import closing
from map import Map
from pathfinder import *
from robot import Robot

# Specify map
print("Map:")
m = Map(4, 7)
m.set_grid(2, 3, 'x')
print(m)

# Explore map sequence
m.set_grid(2, 5, 'o')
unexplored = get_unexplored_grids(m, 1, 5)
print(unexplored)

print(get_nearest_unexplored(map, unexplored, 1, 5))
# seq = get_nearest_unexplored(m,
# print(seq)

# Execute move command in robot
with closing(Robot(6)) as r:
    r.move_sequence(seq)
