from map import Map

# m = Map(10, 10)
# m.set_grid(1, 7, 'x')
# m.set_grid(2, 8, 'x')
# m.set_grid(4, 6, 'x')
# m.set_grid(4, 7, 'x')
# m.set_grid(4, 8, 'x')
# m.set_grid(8, 1, "e")
# m.set_grid(1, 1, "e")
# m.fill(8, 1)

# Map
m = Map(6, 6)
m.set_grid(3, 2, 'x')
m.set_grid(3, 3, 'x')
m.set_grid(3, 4, 'x')
print(m)

# Explore
m.fill(4, 1)

print(m)
print(''.join(m.movement))
print(m.old_coordinate)
print(m.home_grid)
