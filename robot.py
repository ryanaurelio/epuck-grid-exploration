import serial
import time

from contextlib import closing

# Speed
SPEED = 550

# Direction
NORTH = 0
EAST = 1
SOUTH = 2
WEST = 3

class Robot:
    """Robot class"""
    def __init__(self, com_number):
        """Initialize"""
        self.ser = serial.Serial(f'COM{com_number}', 115200, timeout=0)
        self.direction = NORTH
        self.status = 'IDLE'
        self.home_coordinate = (0, 0)
        self.old_coordinate = (0, 0)

    def close(self):
        """Close connection with robot"""
        self.ser.close()

    def step_forward(self):
        # Step forward
        self.ser.write(f'D,{SPEED},{SPEED}\r\n'.encode())
        time.sleep(1)
        self.ser.write(bytes(b'D,0,0\r\n'))
    
        # Delay
        time.sleep(0.2)
    
    def step_backward(self):
        # Step backward
        self.ser.write(f'D,{-SPEED},{-SPEED}\r\n'.encode())
        time.sleep(1)
        self.ser.write(bytes(b'D,0,0\r\n'))
    
        # Delay
        time.sleep(0.2)
    
    def turn_left(self):
        # Turn left
        self.ser.write(f'D,{-SPEED},{SPEED}\r\n'.encode())
        time.sleep(0.58177)
        self.ser.write(bytes(b'D,0,0\r\n'))

        # Update direction
        self.direction -= 1
        self.direction %= 4

        # Delay
        time.sleep(0.2)

    def turn_right(self):
        # Turn right
        self.ser.write(f'D,{SPEED},{-SPEED}\r\n'.encode())
        time.sleep(0.58177)
        self.ser.write(bytes(b'D,0,0\r\n'))

        # Update direction
        self.direction += 1
        self.direction %= 4

        # Delay
        time.sleep(0.2)

    def update_direction(self, new_direction):
        # Fix direction of robot
        
        if self.direction == new_direction:
            return
        elif (self.direction-1)%4 == new_direction:
            self.turn_left()
        elif (self.direction+1)%4 == new_direction:
            self.turn_right()
        else:
            self.turn_left()
            self.turn_left()
    
    def step_north(self):
        self.update_direction(NORTH)
        self.step_forward()
    
    def step_east(self):
        self.update_direction(EAST)
        self.step_forward()
    
    def step_south(self):
        self.update_direction(SOUTH)
        self.step_forward()
    
    def step_west(self):
        self.update_direction(WEST)
        self.step_forward()
    
    def move_sequence(self, seq):
        for c in seq:
            match c:
                case 'W':
                    self.step_north()
                case 'A':
                    self.step_west()
                case 'S':
                    self.step_south()
                case 'D':
                    self.step_east()

with closing(Robot(6)) as r:
    r.move_sequence("WDSASDWAASDW")
