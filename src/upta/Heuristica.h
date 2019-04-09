/** 
 * File:   Heuristica.h
 *
 * Contiene los atributos y prototipos de los metodos de la clase Heuristica.
 * En esta clase se pueden definir distintos tipos de heuristicas que seran
 * utilizadas en el desarrollo de la busqueda.
 *
 */

#ifndef HEURISTICA_H
#define	HEURISTICA_H

#include "common_defs.h"

using namespace std;

class Heuristica {
public:
    Heuristica();
    ~Heuristica();
    static const int EUCLIDEA = 0;
    static const int EUCLIDEA3D = 1;
    static const int OCTIL = 2;
    Heuristica(const int tipo);
    float getH(const POINT* p1, const POINT* p2);
    void setH(const int POINT);
private:
    float (Heuristica::*heuristica) (const POINT*, const POINT*);
    float Euclidea(const POINT* pos, const POINT* dest);
    float Octil(const POINT* pos, const POINT* dest);
};

#endif	/* HEURISTICA_H */

