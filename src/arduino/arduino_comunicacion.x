/* arduino_comunicacion.x: 
 *	RPC code generator with rpcgen.
 */

#ifndef ARDSENSOR_H
#define	ARDSENSOR_H

struct ARDSENSOR {
	char* MACplaca;
	int IDsensor;
};
#endif

#ifndef PTUPOS_H
#define	PTUPOS_H
struct PTUPOS {
	int yaw;
	int pitch;
};
#endif

program ARDUINO_COMUNICACION{
version ARDUINO_COMUNICACIONVERS {
   
            string leerSensor(ARDSENSOR) = 1;
            bool movePTU(PTUPOS)  = 2;
	    
	} = 1;
} = 0x30000005;
