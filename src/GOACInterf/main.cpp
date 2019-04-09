/* 
 * File:   main.cpp
 * Author: Pablo Muñoz
 * Date: 2016
 *
 * GOAC interface with the ISG TurtleBot
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include "nodo_comunicacion.h"

#include "macro-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "mp-pub.h"

/** MP<->GOAC. */
#define ROBOT_NAME "DALA"
#define SERVER_MP "127.0.0.1"
#define PORT_MP 3300
#define PREFIX_CMD "APSI-TREX-CMD"
#define PREFIX_REPLY "APSI-TREX-REPLY"

/** COMMANDS */
#define INIT_ROBOT "InitialiseRobot"
#define MOVE_TO "MoveRobot"
#define MOVE_PTU "MovePTU"
#define TAKE_PIC "TakeSciencePic"
#define COMMUNICATE "Communicate"
/** CMD REPLIES */
#define POSITION "ROBOT-POSITION"
#define PTUPOS "PAN-TILT-POSITION"
#define PICTURE "PICTURE-ID"
#define EMPTY "NIL"


void oprsSendReply(char* client, char* cmd, char* extra, int numr, bool ok)
{
    char mess[128];
    char* stat = (char*)(ok?"OK":"FAIL");
    if(!strcmp(cmd, (char*)EMPTY))
        sprintf(mess,"(%s %d %s %s)",(char*)PREFIX_REPLY, numr, stat, (char*)EMPTY);
    else
        sprintf(mess,"(%s %d %s (%s %s))",(char*)PREFIX_REPLY, numr, stat, cmd, extra);
    send_message_string(mess, client);
}

int oprsRecvCmd(int client, char* sender, char* cmd)
{
    int length;
    char *s, *m;
    s = read_string_from_socket(client, &length);
	m = read_string_from_socket(client, &length);
	strcpy(sender, s);
	strcpy(cmd, m);
    return length;
}

enum COORDS{cx1=0, cy1, cx0, cy0, cang, cdist};
float angulo(float x, float y)
{
    if(x == 0)
    {
        if(y == 0) return 0;
        else return 90;
    }
    else
        return atan(y/x)*57.2957795;
}
float distancia(float x0, float y0, float x1, float y1)
{
    return sqrt(pow(x1-x0,2) + pow(y1-y0,2));
}
int cuadrante(float x, float y)
{
    if(x>=0 && y>=0) return 1;
    if(x<0  && y>=0) return 2;
    if(x<0  && y<0 ) return 3;
    if(x>=0 && y<0 ) return 4;
    return 0;
}
float compensar_angulo(float angulo, int cuadrante)
{
    if(cuadrante == 2) return 180-abs(angulo);
    else if(cuadrante == 3) return -(180-angulo);
    else return angulo;
}


int main(int argc, char** argv)
{
    //MP communication Link
    int client = external_register_to_the_mp_host_prot((char*)ROBOT_NAME, (char*)SERVER_MP, PORT_MP, STRINGS_PT);

    char* turtleip = (char*)"127.0.0.1";
    if(argc < 1)
        std::cout<<"TurtleBot RPC IP not provided, using localhost"<<std::endl;
    else
        turtleip = argv[1];
    CLIENT *clnt;
	DATOS datos_ROS;
    RESULTADO *result;
    PTU_POS ptupos;
    timeval tmout;
    tmout.tv_sec = 120;
    tmout.tv_usec = 0;

    char sender[32], cmd[32];
    char retdat[16], msg[64];
    int len, numcmd = 0;
    char* temptok;
    float coordenadas[6] = {0,0,0,0,0,0};
    float coordprimas[6] = {0,0,0,0,0,0};
    int sigcuadrante = 0;
    do{
        len = oprsRecvCmd(client, sender, msg);
        if(len > 0)
        {
            //std::cout<<ROBOT_NAME<<" recv from "<<sender<<" | MSG["<<len<<"] "<<msg<<std::endl;
            for(int i=0; i<strlen(msg); i++)
                if(!isalnum(msg[i]) && msg[i]!='.' && msg[i]!='-' && msg[i]!='_' && msg[i]!='"') msg[i]=' ';
            temptok = strtok(msg," "); //first token is discarded (APSI-TREX-CMD)
            temptok = strtok(NULL," "); //second too (sender, APSI-TREX)
            temptok = strtok(NULL," "); //command number
            numcmd = atoi(temptok);
            temptok = strtok(NULL," "); //command
            strcpy(cmd, temptok); 
        
            // *** INITIALIZE RPC ***
            if(!strcmp(cmd, (char*)INIT_ROBOT))
            {
               	clnt = clnt_create(turtleip, NODO_COMUNICACION, NODO_COMUNICACIONVERS, "tcp");
	            if(clnt == NULL) 
	            {
                    std::cout<<"Failed to connect to TurtleBot RPC server @"<<turtleip<<std::endl;
		            return -1;
	            }
                std::cout<<"Connected to TurtleBot RPC server @"<<turtleip<<std::endl;

	            clnt_control(clnt, CLSET_TIMEOUT, (char*)&tmout);

	            char *readbaterylevel_1_arg;
	            float *bateryLevel = readbaterylevel_1((void*)&readbaterylevel_1_arg, clnt);
                std::cout<<"> TurtleBot batery level: "<<*(bateryLevel)<<"%"<<std::endl;
	            char *checkchargin_1_arg;
	            int *chargerState = checkchargin_1((void*)&checkchargin_1_arg, clnt);
               	std::cout<<"> TurtleBot batery state: "<<(*(chargerState)? "CHARGING":"DISCHARGING")<<std::endl;
           	   
                oprsSendReply(sender, (char*)EMPTY, NULL, numcmd, true);
            }
            // *** MOVE TO ***
            else if(!strcmp(cmd, (char*)MOVE_TO))
            {
                temptok = strtok(NULL," "); // X pos
                coordenadas[cx1] = atoi(temptok);
                temptok = strtok(NULL," "); // Y pos
                coordenadas[cy1] = atoi(temptok);

                coordprimas[cx1] = coordenadas[cx1]-coordprimas[cx0];
                coordprimas[cy1] = coordenadas[cy1]-coordprimas[cy0];
                coordenadas[cdist] = distancia(0,0, coordprimas[cx1], coordprimas[cy1]);
                sigcuadrante = cuadrante(coordprimas[cx1], coordprimas[cy1]);
                coordenadas[cang] = angulo(coordprimas[cx1], coordprimas[cy1]);
                coordenadas[cang] = compensar_angulo(coordenadas[cang], sigcuadrante);
                coordenadas[cang] -= coordprimas[cang];
                if(abs(coordenadas[cang]) > 180)
                {
                    if(coordenadas[cang] > 180)
                        coordenadas[cang] -= 360;
                    else
                        coordenadas[cang] += 360;
                }

                 std::cout<<"Position: "<<coordprimas[cx0]<<","<<coordprimas[cy0]<<" Heading: "<<coordprimas[cang]<<std::endl; 
                 std::cout<<"Move to "<<coordenadas[cx1]<<","<<coordenadas[cy1]<<std::endl;
                 std::cout<<" Dis: "<<coordenadas[cdist]<<" Ang: "<<coordenadas[cang]<<std::endl;

	            datos_ROS.vel_linear=0.0f;
	            datos_ROS.vel_angular=0.4f;
	            datos_ROS.angulo_rotacion = coordenadas[cang];
	            result = locomotionrotaterqstandrcv_1(&datos_ROS, clnt);
	            if(result == (RESULTADO *)NULL)
		            oprsSendReply(sender, (char*)EMPTY, NULL, numcmd, false);
	
	             std::cout<<" New heading: "<<result->orientacion_actual<<std::endl;
	     
	            datos_ROS.vel_linear=0.12;
	            datos_ROS.vel_angular=0.0f;
	            datos_ROS.avance = coordenadas[cdist];
	            result = locomotionforwardrqstandrcv_1(&datos_ROS, clnt);
	            if(result == (RESULTADO *)NULL)
	                oprsSendReply(sender, (char*)EMPTY, NULL, numcmd, false);
 
                coordprimas[cx0] += coordprimas[cx1];
                coordprimas[cy0] += coordprimas[cy1];
                coordprimas[cang] += coordenadas[cang];
 
                sprintf(retdat, "%d %d 0", (int)(result->pos_actual_x+0.1f), (int)(result->pos_actual_y+0.1f));
                
                std::cout<<" Reached pos: "<<result->pos_actual_x<<","<<result->pos_actual_y<<" Heading: "<<result->orientacion_actual<<std::endl;
                
                oprsSendReply(sender, (char*)POSITION, retdat, numcmd, true);
            }
            // *** MOVE PTU ***
            else if(!strcmp(cmd, (char*)MOVE_PTU))
            {
                temptok = strtok(NULL," "); // pan
                ptupos.yaw = atoi(temptok);
                temptok = strtok(NULL," "); // tilt
                ptupos.pitch = atoi(temptok);
	            int* result = moveptu_1(&ptupos, clnt);

                sprintf(retdat, "%d %d", ptupos.yaw, ptupos.pitch);
                oprsSendReply(sender, (char*)PTUPOS, retdat, numcmd, (*result==1));
            }
            // *** TAKE PICTURE ***
            else if(!strcmp(cmd, (char*)TAKE_PIC))
            {
                temptok = strtok(NULL," "); // Photo ID
                strcpy(retdat, temptok);
                char *takepicture_1_arg;
	            int *takepictureState = takepicture_1((void*)&takepicture_1_arg, clnt);

                oprsSendReply(sender, (char*)PICTURE, retdat, numcmd, true);
            }
            // *** COMMUNICATE ***
            else if(!strcmp(cmd, (char*)COMMUNICATE))
            {
                sleep(10);
                oprsSendReply(sender, (char*)EMPTY, NULL, numcmd, true);
            }

        }//if len>0
    }while(len>0);

/*	char *autodocking_1_arg;
	int *autodockingState = autodocking_1((void*)&autodocking_1_arg, clnt);
	printf("BATERY STATE: %s\n",((char*)(*(autodockingState)? "AUTODOCKING OK":"AUTODOCKING FAIL")));
*/

    return 0;
}
