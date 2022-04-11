# e-puck 2 Grid Exploration

A project for exploring a map as grids using e-puck 2. The robots will use sound for communication encoded using
different range of frequencies. 

First, the robot will enter an initialization phase to register all the available robots. After the initialization phase
is complete, the robots will explore the map. While exploring, they will use sound to communicate which grid they will
explore. Each robot will keep track which grid is already explored.

Features from e-puck2 that we use:
<ul>
  <li> Microphone </li>
  <li> Speaker </li>
  <li> Motor </li>
  <li> LED </li>
</ul>

<h2>There are two separate program that we uploaded here </h2>
<ol>
  <li> program with fixed map </li>
  <li> program with non-fixed map </li>
 </ol>
<h2>How the program works: </h2>
<ol>
  <li> <h3> Connecting Phase </h3>
    <p> The connecting phase for both of programs are the same.
      Here, the robot will produce a sound(~2100Hz) for 1 second and wait 5 seconds for a reply(~2100Hz) for another robot. The LED2 indicates the time for the robot to hear the reply. <br>
      If the robot hears the sound in this 5 seconds, the robot will register the sound as earlier robot (robot number ID = 1, robot number ID = 2, ..). LED1 will be used as an indicator for registering the robot. <br>
      After 5 seconds has elapsed, the robot will register itself to the list of the robots and then the robot will wait again for 5 seconds to respond to another robot. LED4 will be used as an indicator for this phase. <br>
    </p>
    <h4> Position </h4>
      <ol>
        <li> <h5> for the fixed map </h5>
          <p>
            the initial position would be (1, ID), where ID is the ID of the robot (starts at 1).
          </p>
        </li>
        <li> <h5> for the non-fixed map </h5>
          <p> the initial position would be (ID - 1, 0), where ID is the ID of the robot (starts at 1).
        </li>
    </ol>
  </li>
  <li> <h3> Working </h3>
    <ol> 
      <li> <h4> for the fixed map </h4> 
        <p> first of all, the robots will check if the map is already completed. If it isn't, the robot will check if it's the robot's turn. If it is, the robot will find the nearest coordinate and broadcast them. By broadcasting the coordinates, the other robots will recognize this robot as not working anymore, and therefore the other robots can start working. After that, the robot will go to that nearest coordinate. After moving, the robot will wait for the situation to be quiet and it will broadcast that it is done working. This process will be repeated until the map is complete.
        </p> 
      </li>
      <li> <h4> for the non-fixed map </h4>
        <p> for the non-fixed map, the robot will make a 360 degree rotation to check for an obstacle in front and for each 90degree rotation, the robot will broadcast if it is an obstacle or a empty path. After that, the process is the same as the fixed map.
        </p>
      </li>
    </ol>
    </li>
  <li> <h3> Listening </h3>
    <ol>
      <li> <h4> for the fixed map </h4>
        <p> The robot will check if the map is complete or not. If it isn't, the robot will try to listen for the sound.
          <br>
          X &emsp;&emsp;&emsp;-&emsp;&emsp;XX&emsp;&emsp; - XX <br>
          robot id - position x - position y <br>
          We encode the sound so that the broadcast has a meaning. The minus point is that, the amount of robots will be 9 at max, and the size of the map will be 16 x 16 max. <br>
          sound frequencies that we use:
        </p>
          <ul>
            <li> 1200Hz - 1400Hz : 0, LED1 will be used as indicator
            </li>
            <li> 1400Hz - 1600Hz : 1, LED3 will be used as indicator
            </li>
            <li> 1600Hz - 1800Hz : 2, LED5 will be used as indicator
            </li>
            <li> 1800Hz - 2000Hz : 3, LED7 will be used as indicator
            </li>
            <li> 2000Hz - 2200Hz : done, body led will be used as indicator. Here, the robot will update the position of another robot.
            </li>
        </ul>
      </li>
      <li> <h4> for the non-fixed map </h4>
        <p> the idea is almost the same, but the sound is divided into binary. <br>
          X &emsp;&emsp;&emsp;- X&emsp;&emsp;&emsp;&emsp;-OX&emsp;&emsp;&emsp;&emsp;-X&emsp;&emsp;&emsp;&emsp;-OX&emsp;&emsp;&emsp;&emsp;-X <br>
          robot id - separator - position x - separator - position y - separator<br>
          <ul>
            <li>
          for the position : OX. These O indicates if the number is positive or negative or it broadcasts done. 00 indicates positive, 01 indicates negative, 11 indicates done.
            </li>
          </ul>
        <br>
        With this, we can also use more than 9 robots and our map can get bigger than 16 x 16. <br>
        sound frequencies that we use:
          <ul>
            <li> 1200Hz - 1500Hz : 0, LED1 will be used as indicator
            </li>
            <li> 1500Hz - 1800Hz : 1, LED3 will be used as indicator
            </li>
            <li> 1800Hz - 2100Hz : done, LED7 will be used as indicator. Here, the robot will update the position of another robot.
            </li>
          </ul>
        </p>
      </li>
    </ol>
  </li>
  </ol>
      
  
