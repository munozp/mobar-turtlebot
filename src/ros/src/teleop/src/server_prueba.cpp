/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h" 

#define PORT 8081
#define FRECUENCY_SLEEP 1000 //Sleep time (1/FRECUENCY_SLEEP)
#define TIME_WHITHOUT_MESSAGES 0.4 //In second
#define DEBUG 1

int sockfd, newsockfd, portno;

void error(const char *msg)
{
perror(msg);
exit(1);
}

/** Capture SIGINT to close client */
void ctrlc(int sig)
{
    std::cout << "Closing connection" << std::endl;  
    close(newsockfd);
    close(sockfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    n=1;


    geometry_msgs::Twist velocity;
    ros::Publisher publish_vel;
    double values_speed[5];
    char * aux;
    int i=0;
    
    ros::Time begin;// Init counter time
    ros::Duration retardment(TIME_WHITHOUT_MESSAGES);

    ros::init(argc, argv, "Server_node"); //Put on the server node
    ros::NodeHandle na;

    publish_vel=na.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop", 1);
    ros::Rate loop_rate(FRECUENCY_SLEEP);
    signal(SIGINT, &ctrlc);// Capture sigint to close the server
    

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno =PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
    sizeof(serv_addr)) < 0)
    error("ERROR on binding");
    while (n>0)
    {
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
        (struct sockaddr *) &cli_addr,
        &clilen);
        if (newsockfd < 0)
        error("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        /******CODIGO ROS*******************************/
        if (n>0)
        {
            std::cout<<"Leido: "<<buffer<<std::endl;
            for(i=0; i<strlen(buffer); i++)
                    if(!isdigit(buffer[i]) && buffer[i]!=',' && buffer[i]!='.') 
                        buffer[i]=' ';

               aux=strtok(buffer,",");
               values_speed[0]=atof(aux);

               for (i=1; i<6; i++)
               {
                    aux=strtok(NULL, ",");
                    values_speed[i]=atof(aux);
               }
                velocity.linear.x =values_speed[0];   //Forward velocity
                velocity.angular.z = values_speed[5]; //Turn velocity 
    
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
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);
        n = write(newsockfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");
    return 0;
}
