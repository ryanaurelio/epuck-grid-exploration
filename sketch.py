import serial
import time
ser = serial.Serial('COM11', 115200, timeout=0)

# Speed
SPEED = 550

# Direction
NORTH = 0
EAST  = 1
SOUTH = 2
WEST  = 3

# Current direction (start with north)
direction = NORTH

def step_forward():
    # Step forward
    ser.write(f'D,{SPEED},{SPEED}\r\n'.encode())
    time.sleep(1)
    ser.write(bytes(b'D,0,0\r\n'))

    # Delay
    time.sleep(0.2)

def step_backward():
    # Step backward
    ser.write(f'D,{-SPEED},{-SPEED}\r\n'.encode())
    time.sleep(1)
    ser.write(bytes(b'D,0,0\r\n'))

    # Delay
    time.sleep(0.2)

def turn_left():
    # Turn left
    ser.write(f'D,{-SPEED},{SPEED}\r\n'.encode())
    time.sleep(0.58775)
    ser.write(bytes(b'D,0,0\r\n'))

    # Update direction
    global direction
    direction -= 1
    direction %= 4

    # Delay
    time.sleep(0.2)

def turn_right():
    # Turn right
    ser.write(f'D,{SPEED},{-SPEED}\r\n'.encode())
    time.sleep(0.5875875)
    ser.write(bytes(b'D,0,0\r\n'))

    # Update direction
    global direction
    direction += 1
    direction %= 4

    # Delay
    time.sleep(0.2)

def update_direction(new_direction):
    # Fix direction of robot
    global direction
    if direction == new_direction:
        return
    elif (direction-1)%4 == new_direction:
        turn_left()
    elif (direction+1)%4 == new_direction:
        turn_right()
    else:
        turn_left()
        turn_left()

def step_north():
    update_direction(NORTH)
    step_forward()

def step_east():
    update_direction(EAST)
    step_forward()

def step_south():
    update_direction(SOUTH)
    step_forward()

def step_west():
    update_direction(WEST)
    step_forward()

def move_sequence(seq):
    for c in seq:
        match c:
            case 'W':
                step_north()
            case 'A':
                step_west()
            case 'S':
                step_south()
            case 'D':
                step_east()

# move_sequence("WDSASDWAASDW")
turn_left()
turn_left()
turn_left()
turn_left()

ser.close()
