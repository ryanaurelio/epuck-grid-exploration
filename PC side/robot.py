import serial
import time

# Speed
SPEED = 550

# Direction
NORTH = 0
EAST = 1
SOUTH = 2
WEST = 3

class Robot:

    def __init__(self, com_number: int) -> None:
        """
        Initialize Robot class
        :param com_number: COM port to communicate with e-puck
        """
        self.ser = serial.Serial(f'COM{com_number}', 115200, timeout=0)
        self.direction = NORTH

        self.status = 'IDLE'
        self.home_coordinate = (0, 0)
        self.old_coordinate = (0, 0)

    def close(self) -> None:
        """
        Close the connection with e-puck
        """
        self.ser.close()

    def step_forward(self) -> None:
        """
        Move e-puck forward by one step
        """
        self.ser.write(f'D,{SPEED},{SPEED}\r\n'.encode())
        time.sleep(1)
        self.ser.write(bytes(b'D,0,0\r\n'))
    
        # Delay
        time.sleep(0.2)
    
    def step_backward(self) -> None:
        """
        Move e-puck backward by one step
        """
        self.ser.write(f'D,{-SPEED},{-SPEED}\r\n'.encode())
        time.sleep(1)
        self.ser.write(bytes(b'D,0,0\r\n'))
    
        # Delay
        time.sleep(0.2)
    
    def turn_left(self) -> None:
        """
        Turn e-puck by 90° anti-clockwise
        """
        self.ser.write(f'D,{-SPEED},{SPEED}\r\n'.encode())
        time.sleep(0.58177)
        self.ser.write(bytes(b'D,0,0\r\n'))

        # Update direction
        self.direction -= 1
        self.direction %= 4

        # Delay
        time.sleep(0.2)

    def turn_right(self) -> None:
        """
        Turn e-puck by 90° clockwise
        """
        self.ser.write(f'D,{SPEED},{-SPEED}\r\n'.encode())
        time.sleep(0.58177)
        self.ser.write(bytes(b'D,0,0\r\n'))

        # Update direction
        self.direction += 1
        self.direction %= 4

        # Delay
        time.sleep(0.2)

    def update_direction(self, new_direction: int) -> None:
        """
        Update the direction to where the e-puck will be facing
        :param new_direction: New facing direction for the e-puck
        """
        if self.direction == new_direction:
            return
        elif (self.direction-1)%4 == new_direction:
            self.turn_left()
        elif (self.direction+1)%4 == new_direction:
            self.turn_right()
        else:
            self.turn_left()
            self.turn_left()
    
    def step_north(self) -> None:
        """
        Move e-puck to the north by one step
        """
        self.update_direction(NORTH)
        self.step_forward()
    
    def step_east(self) -> None:
        """
        Move e-puck to the east by one step
        """
        self.update_direction(EAST)
        self.step_forward()
    
    def step_south(self) -> None:
        """
        Move e-puck to the south by one step
        """
        self.update_direction(SOUTH)
        self.step_forward()
    
    def step_west(self) -> None:
        """
        Move e-puck to the west by one step
        """
        self.update_direction(WEST)
        self.step_forward()
    
    def move_sequence(self, seq: str) -> None:
        """
        Parse WASD sequence into corresponding move command
        :param seq: WASD sequence to move e-puck
        """
        for c in seq:
            if c == 'W':
                self.step_north()
            elif c == 'A':
                self.step_west()
            elif c == 'S':
                self.step_south()
            elif c == 'D':
                self.step_east()
