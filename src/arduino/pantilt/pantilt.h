/****************************************************************************************
/* Control de la pantilt del robot 
/* Red de Sensores Asistencia de Ancianos GSI dto Automatica UAH
/* Creado por Daniel Vaquerizo Hernandez, vaquerizo.d@gmail.com 2015
/***************************************************************************************/

#ifndef pantilt_h
#define pantilt_h

#include "../serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class pantilt
{
	public:
		pantilt(speed_t baud);				    //constructor
		~pantilt();					    //destructor
		int position(int angleYaw, int anglePitch);	    //posicion entera
		int yaw(int angleYaw);				    //angulo de giro horizontal
		int pitch(int anglePitch);			    //angulo de elevacion
	private:
		speed_t _baud;		//velocidad de transferencia ex: B57600
		int _angleYaw;		//angulo horizontal
		int _anglePitch;		//angulo elevacion
		int _serial_fd;		//descriptor fichero
                void _flush();           //limpia buffer lectura
                int _writeTo(char * bufferW); //escribe una instruccion puerto serie
};

#endif //pantilt_h
