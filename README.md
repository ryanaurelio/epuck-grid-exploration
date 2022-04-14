# e-puck2 Grid Exploration

A project for exploring a map as grids using e-puck 2. The robots use sound for communication, encoded to different
range of frequencies. This project doesn't require any additional devices to work.

First, the robot will enter a connecting phase to register all the available robots. After the connection phase is
complete, the robots will explore the map. While exploring, they will use sound to communicate which grid they will
explore. Each robot will keep track which grid is already explored.

Features from e-puck2 that we use:
* LEDs
* Motors
* Speakers
* ToF sensor
* Microphones

In the main branch, there are PC side and Robot side implementation of the algorithm. The PC side is in Python and
requires a PC with bluetooth to connect the robots. The PC act as a center entity. On the other hand, the Robot side
development allows the robot to communicate with each other using sound without any center entity. In the Robot side,
we only use the C files since we haven't found a way to assign functions to different threads in C++ using the ChibiOS
library.

## Available Robot Side Programs
1. Exploration with fixed map (indicated with *map*) (See *[Fixed-Map](https://github.com/ryanaurelio/epuck-grid-exploration/tree/Fixed-Map)*)
2. Exploration with non-fixed map (indicated with *dmap*) (See *[Non-Fixed-Map](https://github.com/ryanaurelio/epuck-grid-exploration/tree/Non-Fixed-Map)*)

## Program Description

### 1. Connecting Phase

The connecting phase for both of programs are identical. Here, the robot will produce a sound (~2100Hz) for 1 second and
wait 5 seconds for a reply (~2100Hz) from other robots. The 2nd LED (blue) indicates the time for the robot to hear the
reply. As soon as the receiver robot hears the sound in this time frame, the receiver robot will register the sender
robot as predecessor robot (starting from robot ID = 1). If the receiver robot hears another sound, it will register
this new robot with ID = 2, etc. The 1st LED will be used as an indicator for registering the robot. Everytime, the
receiver robot hears a sound, it will reset the 5 seconds timer.

After 5 seconds has elapsed, the robot will register itself to the list of the robots with new ID and then the robot
will wait again for 5 seconds. In this time frame, when a new robot is turned on, the robot will hear a sound from that
new robot. The robot will also produce a sound for this robot as a response to let the new robot know that there is
other predecessor robots. The 4th LED will be used as an indicator for this phase.

#### Position
* **For the fixed map** 

   The initial position would be (1, ID). To change the size of the map, please refer to *map.h*.

* **For the non-fixed map**

    The initial position would be (ID - 1, 0)

Where ID is theID of the robot (starts at 1).

### 2. Exploring Phase

#### Working

* **For the fixed map**

  First, the robots will check whether the map is already completed. If it's not the case, the robot will check if it's
  the robot's turn (First free robot in the list). If it is, the robot will find the nearest coordinate and broadcast
  it. By broadcasting the coordinate, the other robots will remove this robot from the free robots and mark the target
  coordinate and the path it uses in the map. This is to make sure that there won't be any collision. After broadcasting
  the coordinate, the robot can now move in the map and other robots can also start working. After moving, the robot
  will wait for the situation to be quiet, and it will broadcast that it's done working. The other robots will mark the
  previous path as explored. This process will be repeated until the map is complete.


* **For the non-fixed map**

  The only difference here is the robot will make a 360° rotation before exploring to check for an obstacle in front of
  the robot and for each 90° rotation. The robot will then broadcast if it is an obstacle or an unexplored grid. After
  the broadcast, all robots will mark the map according to the information sent.

#### Listening

* **For the fixed map**

  The robot will check whether the map is complete. If it isn't, the robot will try to listen for the sound. The robot
  will hear 5 *beeps* as the following:

  | X        | XX         | XX         |
  |----------|------------|------------|
  | Robot ID | Position x | Position y |

  Sound frequencies that we use:
  * 1200Hz - 1400Hz : 0 (indicated by LED1)
  * 1400Hz - 1600Hz : 1 (indicated by LED3)
  * 1600Hz - 1800Hz : 2 (indicated by LED5)
  * 1800Hz - 2000Hz : 3 (indicated by LED7)
  * 2000Hz - 2200Hz : Done working indicator (Indicated by body LED)
  

* **For the non-fixed map**

  The difference is we use separator here, and we encode the information in binary due to the instability when using
  more frequency ranges.

  | X        | X         | OX         | X         | OX         | X         |
  |----------|-----------|------------|-----------|------------|-----------|
  | Robot ID | Separator | Position x | Separator | Position y | Separator |

  Where O indicates the number sign or when the broadcast is done.
  * 00 : Positive sign (+)
  * 01 : Negative sign (-)
  * 11 : Broadcast done

  Sound frequencies that we use:
  * 1200Hz - 1500Hz : 0 (indicated by LED1)
  * 1500Hz - 1800Hz : 1 (indicated by LED3)
  * 1800Hz - 2100Hz : Done working indicator (indicated by LED7)
