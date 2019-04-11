# MoBAr: Model-Based Architecture
This is the 'stable' deployment of the Model-Based Architecture (MoBAr). MoBAr is an autonomous robotic controller that allows on-board planning and replanning for goal oriented autonomy. It relies on technologies such as PLEXIL to model the execution behaviours, or the action oriented planning language PDDL for the domain definition and the planning process. The current functional layer to control the robotic platform is based on ROS. Using these technologies MoBAr enables an easier deployment of the autonomous controller for different robotics platforms.

# Resources
MoBAr has three main deploys:
- The first version works using [GenOM](https://www.openrobots.org/wiki/genom) for the functional layer and operates the ESA's ExoMars rover simulator (3DROV). This version is not available as part of the software is propietary. A demonstration video can be seen on [Youtube: MoBAr ExoMars](https://www.youtube.com/watch?v=TiOf5CBca0M&t=44s).
- A modified version for a 5 DoF robotic manipulator was implemented as a degree dissertation. You can find the code in the ISG repository [MoBAr manipulator](https://github.com/ISG-UAH/mobar-manipulator) and a video on [Youtube: MoBAr manipulator](https://www.youtube.com/watch?v=QEVBsY89R-U).
- The version implemented in this repository uses [ROS](ros.org) to control a modified TurtleBot 2 platform. This modification adds a PTU with an HD camera (a Raspberry Pi camera) and a HC-SR04 ultrasonic sensor, 2 more ultrasonic sensors on the body (pointing to the front), a wide-angle camera between them and we added some specific support for telepresence for the [LARES project](https://www.youtube.com/watch?v=6cSWPd1M_pA). You can see the final assembly in the [ISG LARES Flicker](https://www.flickr.com/photos/isg-uah/albums/72157685872428622).

If you are interesting in using the architecture, learn more or to cite it in your work, the MoBAr research article is published in the Journal of Intelligent Robotics & Systems: [MoBAr: a Hierarchical Action-Oriented Autonomous Control Architecture](https://doi.org/10.1007/s10846-018-0810-z).


# Requirements
Each layer of the architecture requires specific software. Here are only presented direct dependencies:
- Deliberative: if you have an executable planner (we use PDDL planners) you do not need to install anything. In this version there is a variation of the [UP2TA planner](https://github.com/ISG-UAH/up2ta) adapted to work with PDDL 2.0 (or higher) planners. To use such version you need a PDDL planner as an executable, the modified UP2TA is pure C++ code that can be compiled with standard libraries. If you are interested in such planner, the sources are located in src/up2ta, being the main.cpp the entry point of the planner. In the Makefile the rule ```up2ta``` will compile the planner. You will need to modify some constants to point to your PDDL planner. The path planning algorithm is [Theta*](https://doi.org/10.1613/jair.2994). Please note that the code is in spanish and currently I have not plans to translate it.
- Executive: the executive uses [PLEXIL](https://github.com/nasa/PLEXIL5). The architecture evolves from initial versions of PLEXIL (previous to 1.0) an this last one works with PLEXIL 2.0 (probably with higher versions, but that has not tested). 
- Functional layer: the current implementation is based on the TurtleBot packages for ROS Indigo. Also, some specific code for controlling other assemblies that require Arduino can be found.


# Bulding
To build use the Makefile in the root directory. Doxygen documentation can be created using the mobar file in the same folder.


# Executing
To execute it, there are currently two steps that are executed by the run.sh script in the bin directory:
1. Execute the ROS support.
2. Execute the architecture with the PLEXIL executive (which internally executes the planner when required).

All PDDL and PLEXIL files are located in the bin folder. Changing them allows you to modify the behaviours of the architecture.


# Notes
You can find a weird code under src/GOACInterf. That code enables our TurtleBot to be controlled using the [ESA's GOAC autonomous controller](http://esa-tec.eu/space-technologies/from-space/goal-oriented-autonomous-controller-goac/).

If you find code related to OGATE, you can simply rip off it. That code enables monitoring services for the [ESA's OGATE tool](https://www.esa.int/Our_Activities/Space_Engineering_Technology/Virtual_workout_designed_for_rover_control_systems). Unluckly, that tool is no available but in the future a improved version will be implemented as free software. A video demonstration of the tool can be seen in [Youtube: OGATE](https://www.youtube.com/watch?v=sqF0uFA1w_8).
