from contextlib import closing
from map import Map
from pathfinder import explore
from robot import Robot

# Specify map
print("Map:")
m = Map(4, 7)
m.set_grid(2, 3, 'x')
print(m)

# # Explore map sequence
# seq = explore(m, 4, 1)
# print(seq)

# Execute move command in robot
with closing(Robot(6)) as r:
    r.move_sequence(seq)
