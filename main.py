from contextlib import closing
from robot import Robot

with closing(Robot(6)) as r:
    r.move_sequence("WDSASDWAASDW")
