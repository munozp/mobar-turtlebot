/* nodo_comunicacion.x: 
 *	RPC code generator with rpcgen.
 *
 */

/*
 *	velStruct.h definitions
 */

#ifndef DATOS_H
#define	DATOS_H

struct DATOS {
	float vel_linear;
	float vel_angular;
	float angulo_rotacion;
	float avance;

};
#endif


#ifndef RESULTADO_H
#define	RESULTADO_H
struct RESULTADO {
	float pos_actual_x;
	float pos_actual_y;
	float orientacion_actual;
	int error;
};
#endif
/*
 *	functions to implement
 */
program NODO_COMUNICACION{
version NODO_COMUNICACIONVERS {
   
            RESULTADO locomotionForwardRqstAndRcv(DATOS) = 1;
            RESULTADO locomotionRotateRqstAndRcv(DATOS)  = 2;
	    int checkBumperSemsorState(void) =3;
	    float readBateryLevel(void)=4;
	    int   checkChargin(void)=5;
	    int   autodocking (void)=6;
	    int  takePicture(void)=7;
	    RESULTADO checkPosition(void)=8;
        int restartDockServer(void)=9;
        int stopAutodock(void)=10;
        int stopAction(char * command)=11;
	    
	} = 1;
} = 0x20000011;
