#ifndef CPDDLstruct_H
#define CPDDLstruct_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include <unistd.h>
using namespace std;


/** Caracter inicial para comentar una linea en el archivo de configuracion. */
#define COMENTARIO '%'
/** Cadena para indicar el inicio de configuracion de acciones en el archivo de configuracion. */
#define CONFIG_ACCIONES "ACTIONS#"
/** Cadena a utilizar para indicar el final del plan. */
#define FINAL_PLAN "FINAL_PLAN"
/** Cadena a utilizar para indicar que el plan no ha sido abierto. */
#define PLAN_NO_OPEN "FAIL_OPEN_PLAN"
/** Parametro para realizar la replanificacion. */
#define REPLANIFICAR true
/** Numero maximo de objetos para la estructura stpredicado o parametros de staccion. */
#define MAX_OBJ_PRED 9
/** Tamaño maximo para la cadena que representa un objeto o un parametro. */
#define MAX_CAR 64
/** Longitud maxima de los ficheros de entrada o de las rutas. */
#define TAM_LINEA 512
/** Maximo de lineas a leer para iniciar ordenes en el plan. */
#define MAX_LINEA_ACCION 64



typedef struct{
    /** Nombre del predicado. */
	char nombre[MAX_CAR];
    /** Objetos del predicado. */
	char** objetos;
    /** Para funciones o duraciones*/
    float valor;
/** Estructura para almacenar predicados, funciones o acciones. */
}stpredicado;


typedef struct{
    /** Nombre de la accion. */
	char nombre[MAX_CAR];
    /** Ruta al plan PLEXIL equivalente. */
	char rutaPlexil[MAX_CAR];
    /** 0-token de accion 1-numero de objetos 2-token duracion. Se comienza a contar por 1.*/
    int tokens[3];
    /** Duracion de la accion, sin duracion: -1. */
	float duracion;
//        /** Uso futuro. */
//	char parametros[MAX_OBJ_PRED][MAX_CAR];
/** Estructura para almacenar acciones y la correspondenica accion->plan PLEXIL. */
}staccion;


#endif // CPDDLstruct_H
