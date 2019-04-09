/****************************************************************************************
/* Control de la pantilt del robot 
/* Red de Sensores Asistencia de Ancianos GSI dto Automatica UAH
/* Creado por Daniel Vaquerizo Hernandez, vaquerizo.d@gmail.com 2015
/***************************************************************************************/

#include "pantilt.h"

/* constructor. int baudrate es la velocidad de conexion con Arduino por el puerto serie */
/* angleYaw= angulo horizontal. anglePitch = angulo de elevacion */
pantilt::pantilt(speed_t baud){
	char port[100];
	FILE *fp;
	char command[50];	

	fp = popen("/bin/ls /dev/ttyACM*", "r");
	if(fp == NULL){
		printf("COM: POPEN: Error executing command\n");
		exit(-1);
	}

	fscanf(fp, "%s", port);
	//printf("COM: Port %s.", port);
	
	_serial_fd = serial_open(port, baud);

	if (_serial_fd==-1) {
		printf("COM: OPEN: Error connecting with Arduino.\n");
		exit(-2);
	}else{

		//printf("COM: Communication starts. Port %d open.", _serial_fd);
		_baud = baud;
	}
        _flush();
}
/********************************************/



/* destructor. cierra el dispositivo */
pantilt::~pantilt(){
	serial_close(_serial_fd);
}
/********************************************/



/* situa la pantilt en la posicion dada, angleYaw = angulo horizontal, anglePitch = angulo elevacion */
int pantilt::position(int angleYaw, int anglePitch){
	char command[100];
	char command2[100];
	int recv = 0;

	if((angleYaw<20) || (angleYaw>160)){printf("ERROR:Angulo fuera de rango 20<=angulo<=160"); return -1;}
	if((anglePitch<70) || (anglePitch>145)){printf("ERROR:Angulo fuera de rango 70<=angulo<=145"); return -1;}

	//mandamos angulo Yaw y angulo Pitch
	sprintf(command, "Yaw %d:Pitch %d;", angleYaw, anglePitch);
	_writeTo(command);
	
	recv = serial_read(_serial_fd, command2, strlen(command), 1000000);

	if(strcmp(command, command2)) return -1;

	_angleYaw = angleYaw;
	_anglePitch = anglePitch;

	return 0;
}
/********************************************/



/*situa yaw*/
int pantilt::yaw(int angleYaw){
	char command[100];
	char command2[100];
	int recv = 0;

	if((angleYaw<20) || (angleYaw>160)){printf("ERROR:Angulo fuera de rango 20<=angulo<=160"); return -1;}

	//mandamos angulo Yaw
	sprintf(command, "Yaw %d;", angleYaw);
	_writeTo(command);
	
	recv = serial_read(_serial_fd, command2, strlen(command), 1000000);

	if(strcmp(command, command2)) return -1;

	_angleYaw = angleYaw;

	return 0;
}
/********************************************/



/*situa pitch*/
int pantilt::pitch(int anglePitch){
	char command[100];
	char command2[100];
	int recv = 0;

	if((anglePitch<70) || (anglePitch>145)){printf("ERROR:Angulo fuera de rango 70<=angulo<=145"); return -1;}

	//mandamos angulo Yaw
	sprintf(command, "Pitch %d;", anglePitch);
	_writeTo(command);
	
	recv = serial_read(_serial_fd, command2, strlen(command), 1000000);

	if(strcmp(command, command2)) return -1;

	_anglePitch = anglePitch;

	return 0;
}
/********************************************/



/* flush() limpia buffer de lectura*/
void pantilt::_flush(){
    char aux = 0;
    int i = 0;
    
    do{
        i = serial_read(_serial_fd, &aux, 1, 100);
    }while(i!=0);
}
/********************************************/



/* writeTo escribe por el puerto serie*/
int pantilt::_writeTo(char * bufferW){
    char aux = 0;
    int i = 0;
    
    for(i=0; i<strlen(bufferW); i++){
        aux = bufferW[i];
        serial_send(_serial_fd, &aux, 1);
        if(i%9==0){
            usleep(10000); //espera para evitar cuello de botella
        }
    }
    return strlen(bufferW);
}
/********************************************/