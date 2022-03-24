# e-puck 2 Grid Exploration

A project for exploring a map as grids using e-puck 2. The robots will use sound for communication encoded using
different range of frequencies. 

First, the robot will enter an initialization phase to register all the available robots. After the initialization phase
is complete, the robots will explore the map. While exploring, they will use sound to communicate which grid they will
explore. Each robot will keep track which grid is already explored.

Features from e-puck2 that we use:
<ul>
  <li> ToF sensor </li>
  <li> Microphone </li>
  <li> Speaker </li>
  <li> Motor </li>
  <li> LED </li>
</ul>

<h2>How the program works: </h2>
<ol>
  <li> <h3> Connecting Phase </h3>
    <p> Here, the robot will produce a sound and wait for a reply for another robot. The LED2 indicates the time for the robot to hear the reply.
      After that, the robot 
  
