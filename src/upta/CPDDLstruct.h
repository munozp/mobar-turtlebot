#ifndef CPDDLstruct_H
#define CPDDLstruct_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <string> 
using namespace std;

/** Numero maximo de objetos para la estructura stpredicado o parametros de staccion. */
#define MAX_OBJ_PRED 9
/** Tamaño maximo para la cadena que representa un objeto o un parametro. */
#define MAX_CAR 31
/** Longitud maxima de los ficheros de entrada o de las rutas. */
#define TAM_LINEA 1024
/** Cadena a utilizar para indicar el final del plan. */
#define FINAL_PLAN "FINAL_PLAN"
/** Cadena a utilizar para indicar que el plan no ha sido abierto. */
#define PLAN_NO_OPEN "FALLO_APERTURA_PLAN"
/** Cadena que precede al inicio de las acciones del plan. */
#define INICIO_ACCIONES "; Cost:" 
//#define INICIO_ACCIONES "; PlanningTechnique Modified-FF"
/** Maximo de lineas a leer para iniciar ordenes en el plan. */
#define MAX_LINEA_ACCION 25
/** Predicado de accion de movimiento en el plan generado. */
/*******************CON OPTIC DEJAR ESTO***********************************************/
#define ACCION_MOVE "moveto"
#define ACCION_PTU "moveptu"
#define ACCION_DOCK "autodock"
#define ACCION_UNDOCK "undock"
#define ACCION_PICT "takepicture"/***************************/
/*********************************CON SGPlan*********************
#define ACCION_MOVE "MOVETO"
#define ACCION_PTU "MOVEPTU"
#define ACCION_DOCK "AUTODOCK"
#define ACCION_UNDOCK "UNDOCK"
#define ACCION_PICT "TAKEPICTURE"/*
/** Posicion donde se encuentra la accion en el plan generado. */
#define TOKEN_ACCION 2
/** Numero de objetos de accion de movimiento. */
#define NOBJS_MOVE 3
#define NOBJS_PTU 4
#define NOBJS_DOCK 2
#define NOBJS_PICT 5
/** Definicion del predicado para la funcion de distancia PDDL. */
#define FUNCION_DISTANCIA "distance_to_move"
/** Predicado para nuevas metas PDDL. */
#define PREDICADO_NOVISTO "not-visited"
/** Nombre del robot en el archivo PDDL. */
#define nombre_robot "turtle"
#define NOMBRE_ROBOT "turtle"


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
	char rutaPlexil[TAM_LINEA];
        /** 0-token de accion 1-numero de objetos 2-token duracion. Se comienza a contar por 1.*/
    int tokens[2];
        /** Duracion de la accion, sin duracion: -1. */
	float duracion;
/** Estructura para almacenar acciones y la correspondenica accion->plan PLEXIL. */
}staccion;

#endif // CPDDLstruct_H
