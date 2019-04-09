PPD=bin/plexil

#ARDUINO RPC
# bin/arduino_rpc_sever &

#ROS & turtle RPC
if ps ax | grep roslaunch > /dev/null 
then
#    roslaunch turtlebot_bringup minimal.launch &
#    sleep 20
echo "FIXME"
else
    echo "ROS running."
fi
if ps ax | grep ros_rpc_server > /dev/null
then
#    rosrun nodo_comunicacion ros_rpc_server &
#    sleep 3
echo "FIXME"
else
    echo "ROS RPC server running."
fi

#PLEXIL
plexilexec -p $PPD/INIT.plx -l $PPD/Planner.plx -l $PPD/Locomotion.plx -c $PPD/config.xml -d $PPD/Debug.cfg

