#include "ros/ros.h"
#include "teleop/ClientSelect.h"

#define PORT 8081
#define DURATION 5    //In sec
#define SLEEP_WHILE 1 //In Hz

int main(int argc, char** argv) 
{
    ros::init(argc, argv, "Client_node");
    ros::NodeHandle nh;
    ROS_INFO("Client node OK");
    ROS_INFO("Proceed to open client");
    char cadena_enviar[50]="Hola soy Cliente";
    char ip[10]="127.0.0.1";
    ClientSelect cliente;
    int devuelto;
    

    ros::Rate loop_rate(SLEEP_WHILE); //Frecuencia de realización del bucle while.
    ros::Time ahora; // variable de tipo tiempo para almacenar el instante de tiempo.
    if(cliente.open(ip,PORT))
    {
        ROS_INFO("Client OK");
    }
    else
    {
        ROS_INFO("Client ERROR");
        return -1;
    }
    if (devuelto==-1)
    {
        ROS_INFO("Error al enviar datos");
        return -1;
    }
    ahora=ros::Time::now();
    while(ros::Time::now()<ahora + ros::Duration(DURATION))
	{
        std::cout<<"Envío datos"<<std::endl;
		devuelto=cliente.send("[1.0,2.0,3.0,4.0,5.0,6.0]");
        loop_rate.sleep(); 
	}
    
}
