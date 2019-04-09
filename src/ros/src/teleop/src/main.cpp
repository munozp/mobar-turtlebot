#include <cstdlib>
#include <iostream>
#include <csignal>
#include <stdio.h>
#include <string.h>
#include "ros/ros.h"
#include "teleop/ServerSelect.h"
#include "geometry_msgs/Twist.h" 

#define PORT 8081
#define FRECUENCY_SLEEP 1000 //Sleep time (1/FRECUENCY_SLEEP)
#define TIME_WHITHOUT_MESSAGES 0.4 //In second
#define DEBUG 1

using namespace std;

ServerSelect server;

/** Capture SIGINT to close client */
void ctrlc(int sig)
{
    std::cout << "Closing connection" << std::endl;  
    close(server.getSocket());
    exit(0);
}

/** Print how to use and exit. */
void usage()
{
    std::cout << "Usage: servertest PORT" << std::endl;
    exit(-1);
}


int main(int argc, char** argv) 
{

    geometry_msgs::Twist velocity;
    ros::Publisher publish_vel;
    int port;
    char message[M_LENGTH];
    double values_speed[5];
    int read = 1;
    char * aux;
    int i=0;
    
    ros::Time begin;// Init counter time
    ros::Duration retardment(TIME_WHITHOUT_MESSAGES);

    ros::init(argc, argv, "Server_node"); //Put on the server node
    ros::NodeHandle n;
    port =PORT; //Comunnication port
    signal(SIGINT, &ctrlc);// Capture sigint to close the server    
    if (DEBUG)
        std::cout << "Opening localhost server at "<<port<<" ... ";
    if(!server.open(port))
    {
        std::cout << "ERROR: cannot open server" << std::endl;
        return -1;
    }
    if (DEBUG)
        std::cout << "server open" << std::endl;

    publish_vel=n.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop", 1);

    ros::Rate loop_rate(FRECUENCY_SLEEP);
    while(read > -1)
    {
        read = server.receive(message);
        if(read > 1)
        {
           if (DEBUG)
                std::cout <<"Message received: "<< message << endl;
        
           for(i=0; i<strlen(message); i++)
                if(!isdigit(message[i]) && message[i]!=',' && message[i]!='.') 
                    message[i]=' ';

           aux=strtok(message,",");
           values_speed[0]=atof(aux);

           for (i=1; i<6; i++)
           {
                aux=strtok(NULL, ",");
                values_speed[i]=atof(aux);
           }
            velocity.linear.x =values_speed[0];   //Forward velocity
            velocity.angular.z = values_speed[5]; //Turn velocity  
            if (DEBUG)
            {
                std::cout<<"Read Value: "<<read<<std::endl;
                std::cout <<"Speed values received: ";
                for(i=0; i<6; i++)
                    std::cout<<values_speed[i]<<" "; 
                std::cout<<std::endl;
            }
        begin=ros::Time::now();        
        }
        else
        {
            //Not data received, stop if the client dont send data in 400 ms (read=0)
            if((ros::Time::now()-begin) >retardment)
            {
                //Stop the robot
                velocity.linear.x =0;//Forward velocity
                velocity.angular.z =0;//Turn velocity
                if (DEBUG)
                {
                    //std::cout <<"Guard period: "<<retardment.toSec()<<" s"<<std::endl;
                    //std::cout<<"Stop the robot, time without new messages: "<<(ros::Time::now().toSec()-begin.toSec())<<" s"<<std::endl;
                }
            }
            
        }
       publish_vel.publish(velocity); //Publish in the specific topic
       loop_rate.sleep();//Sleep the loop 
    }
    return 0;
}
