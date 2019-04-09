/*
 * @Author: GSI-UAH
 * @Date:   24/03/2012
 *
 * Definicion del tipo nodo y cabeceras comunes empleadas.
 * Version adaptada a UGV (2D) para optimizar y permitir busquedas anidadas exahustivas sin desbordamiento de memoria.
*/

#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <vector>
using std::fstream;
using std::vector;
using std::min;
using std::endl;

/** Factor multiplicativo para calcular el hash del nodo.
Debe ser mayor que la dimension mayor del mapa para evitar hashes duplicados. */
static unsigned short int H_FACTOR = 10000;
/** Separador entre la coordenada X e Y: CYY'char'XX. */
#define POS_DELIMITER "_"


/** 
 * Tipo que define un nodo. Los campos son los siguientes:<br/>
 * (X,Y,Z) coordenadas del nodo<br/>
 * C coste de atravesar la celda correspondiente al nodo<br/>
 * F = G + H ; G = coste de alcanzar el nodo, H = valor de la heuristica. F inicial = -1<br/>
 * parent puntero al nodo padre
 */
typedef struct POINT {
	/** Constructor base */
	POINT(){X=-1; Y=-1; C=0; S=1; F=-1; G=0; H=0; bal=false; }
	/** Constructor con parametros */
	POINT(int x, int y,  int c){X=x; Y=y; C=c; S=1; F=-1; G=0; H=0; bal=false; parent=this;} 
	/** Constructor copia*/
	POINT(const POINT &b){X=b.X; Y=b.Y;	C=b.C; S=b.S; F=b.F; G=b.G; H=b.H;
                          bal=b.bal; parent=b.parent; } 
	/** Coordenada X */
	int X;
	/** Coordenada Y */
	int Y;
	/** Coste de la celda */
	int C;
	/** Factor del margen de seguridad. Mayor o igual a 1. */
	float S;
	/** Funcion F(n) = G(n) + H(n) */
	float F;
 	/** Coste para alcanzar el nodo */
	float G;
	/** Valor de la heuristica del nodo */
	float H;
    /** Indicador si se puede localizar por baliza en ese punto. */
    bool bal;
	/** Padre del nodo */
	POINT* parent;
}POINT;

/** 
 * Igualdad entre nodos por sus coordenadas (X,Y).
 * @param a nodo a
 * @param b nodo b
 * @return true si las coordenadas de a y b son las mismas
*/
inline bool operator==(const POINT &a, const POINT &b) 
{
	return (a.X == b.X && a.Y == b.Y);
}
/** 
 * Desigualdad entre nodos por sus coordenadas (X,Y).
 * @param a nodo a
 * @param b nodo b
 * @return true si las coordenadas de a y b son distintas
*/
inline bool operator!=(const POINT &a, const POINT &b) 
{
	return (a.X != b.X || a.Y != b.Y);
}
/** Hash para incluir el punto en estructuras indexadas.
 * @param a nodo para calcular su hash
 * @return hash del nodo de la forma: X + Y*H_FACTOR
 * @see H_FACTOR
*/
inline unsigned int hash(const POINT &a)
{
	return a.X + a.Y * H_FACTOR;
}

/** Comparacion por la funcion F. Si ambas F son iguales se compara por G (desempate).
 * @param a nodo a
 * @param b nodo b
 * @return devuelve true si F para el nodo a es menor que b, falso en caso contrario.
 */
inline bool operator<(const POINT &a, const POINT &b) 
{
	if(a.F != b.F)
		return a.F > b.F;
	else
		return a.G > b.G;
}
/** 
 * Actualiza los valores G y H del nodo y establece su valor F.
 * @param n nodo a actualizar
 * @param g valor g, mayor o igual a 0 (coste de alcanzar el nodo)
 * @param h valor h, mayor o igual a 0 (valor de la heuristica)
 */
inline void setF(POINT *n, const float g, const float h)
{
	if(g >= 0 && h >= 0)
	{
		n->G = g;
		n->H = h;
		n->F = g + h;
	}
}
/**
 * Establece el padre del nodo.
 * @param p nodo a modificar
 * @param t nuevo padre del nodo p
 */
inline void setParent(POINT *p, POINT *t)
{
	p->parent = t;
}
/**
 * Muestra los datos del nodo dado por pantalla.
 * @param n nodo cuya informacion se desea mostrar
 */
inline void printPoint(const POINT &n)
{
	std::cout << "("<<n.X<<","<<n.Y<<")["<<n.C<<"] "<<n.F<<" = "<<n.G<<"+"<<n.H<< endl;
}

/**
 * Muestra las coordenadas del nodo dado por pantalla usando la resolucion del mapa.
 * @param n nodo cuya informacion se desea mostrar
 * @param res resolucion, factor multiplicativo para las coordenadas
 */
inline void printPointRes(const POINT &n, const float res)
{
	std::cout <<n.X*res<<" "<<n.Y*res<<endl;
}

#endif //COMMON_DEFS
