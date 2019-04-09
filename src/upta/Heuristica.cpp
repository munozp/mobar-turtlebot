#include "Heuristica.h"

Heuristica::Heuristica(){
}

/**
 * Contructor.
 * Establece la funcion heuristica que se va a utilizar en el 
 * proceso de busqueda de una solucion para el problema.
 * @param tipo Tipo de funcion heuristica.
 */

Heuristica::Heuristica(const int tipo) {
    if (tipo == Heuristica::EUCLIDEA){
        heuristica = &Heuristica::Euclidea;
    }
    if (tipo == Heuristica::OCTIL){
        heuristica = &Heuristica::Octil;
    }
}

/**
 * Deconstructor.
 * Libera la memoria ocupada por el objeto.
 */

Heuristica::~Heuristica() {
}

/**
 * Permite modificar la funcion heuristica que se va a utilizar en el 
 * proceso de busqueda de una solucion para el problema.
 * @param tipo Tipo de funcion heuristica.
 */

void Heuristica::setH(const int tipo){
    if (tipo == Heuristica::EUCLIDEA){
        heuristica = &Heuristica::Euclidea;
    }
    if (tipo == Heuristica::OCTIL){
        heuristica = &Heuristica::Octil;
    }
}

/**
 * Implementa la funcion que calcula la heuristica euclidea.
 * @return Valor heuristico.
 */

float Heuristica::Euclidea(const POINT* pos, const POINT* dest)
{
	return sqrt( pow(dest->X - pos->X, 2) + pow(dest->Y - pos->Y, 2) );
}


/**
 * Implementa la funcion que calcula la heuristica octil.
 * @return Valor heuristico.
 */

float Heuristica::Octil(const POINT* pos, const POINT* dest)
{
	int dif_x, dif_y;
	int largest, smallest;
	dif_x = abs(pos->X - dest->X);
	dif_y = abs(pos->Y - dest->Y);
	largest = max(dif_x, dif_y);
	smallest = min(dif_x, dif_y);
	return sqrt(2) * smallest + (largest - smallest);
}

/**
 * Devuelve el valor heuristico que hay entre dos puntos
 * que se pasan como argumento.
 * @return Valor heuristico.
 */

float Heuristica::getH(const POINT* p1, const POINT* p2) {
    return (this->*heuristica)(p1, p2);
}


