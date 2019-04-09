#ifndef GSISEARCH_H
#define GSISEARCH_H

#include <cfloat>
#include <map>
#include "Map.h"
using std::map;
using std::pair;

/**
 * Busqueda de una ruta usando el algoritmo Basic Theta* (Nash et al. 2010) con algunas modificaciones.
 * La utilizacion de costes transversales dependera de los parametros empleados.
 */
class GSIsearch {

	public:
		/**
		 * Constructor. Solo establece los parametros internos en base a los dados.
		 * @param dem puntero a la clase con el mapa donde se buscara la ruta
		 * @param cost indica si se deben tener en cuenta los costes transversales
		 * @param dbg indica si se debe mostrar la informacion de depuracion durante la busqueda
		 */
		GSIsearch(Map *dem, bool cost = false, bool dbg = false);
		/**
		 * Destructor. Elimina las estructuras de las listas de abiertos y cerrados.
		 */
		~GSIsearch();
		/**
		 * Limpia las estructuras de las listas de abiertos y cerrados y reinicia los datos internos.
		 */
		void clear_data();
		/**
		 * Realiza la busqueda de la ruta entre los nodos dados. Si encuentra la ruta realiza el camino siguiendo
		 * los punteros a los nodos padres desde el final hasta el inicio, guardando el resultado en <i>path</i>.
		 * @param start nodo inicial
		 * @param goal nodo final
		 * @return verdadero si ha encontrado una ruta, falso en caso de expandir todos los nodos posibles y no 
		 * alcanzar el nodo final o goal.
		 */
		bool search(POINT start, POINT goal);
		/**
		 * Devuelve un puntero al vector con la ruta obtenida (con el nodo objetivo en la posicion [0]).
		 * Si se usa el modo de depuracion ademas muestra la ruta en el orden natural.
		 * @return un puntero al vector con la ruta del nodo goal al nodo start
		 */
		vector<POINT*> get_path();
		/**
		 * Devuelve la distancia recorrida de la ruta encontrada.
		 * @return el coste de la ruta, -1 si no se ha encontrado solucion.
		 */
		float get_dist();
		/**
		 * Devuelve el coste de la ruta encontrada.
		 * @return el coste de la ruta, -1 si no se ha encontrado solucion.
		 */
		float get_cost();
		/**
		 * Actualiza la lista de abiertos y correspondientemente el valor de F y el padre de succ en funcion 
		 * de la linea de visibilidad entre los nodos pos y succ y el valor F de succ. 
		 * @param pos puntero al nodo con la posicion actual
		 * @param succ puntero al nodo con el succesor de pos a tratar
		 * @param goal nodo objetivo (para calcular la heuristica)
		 */
		void update_vertex(POINT* pos, POINT* succ, POINT* goal);
		/**
		 * Permite calcular el coste exacto para un segmento usando costes transversales. Calcula ademas si hay o no
		 * linea de visibilidad entre los dos nodos dados. 
		 * @param pos nodo de origen
		 * @param succ nodo de destino
		 * @return un valor mayor a 0 si hay linea de visiblidad (en ese caso el valor sera el coste para alcanzar
		 * succ desde pos) o -1 si no hay linea de visibilidad.
		 */
		float segment_cost(POINT* pos, POINT* succ);
		/**
		 * Calcula los puntos de corte con la cuadricula entre dos nodos. Se usa para el calculo de costes.
		 * @param pos nodo de origen
		 * @param succ nodo de destino
		 * @return una lista con coordenadas (X,Y) que representan los puntos de corte con los ejes.
		 */
		vector<pair<float,float> > axis_points(POINT* pos, POINT* succ);
		/**
		 * @return devuelve el numero de nodos expandidos durante la busqueda.
		 */
		unsigned int expanded_nodes();
		/**
		 * Muestra por pantalla la lista de abiertos (ordenada de menor a mayor F).
		 */
		void print_open();
		/**
		 * Muestra por pantalla la lista de cerrados.
		 */
		void print_closed();

	private:
		/**	Verdadero para usar costes tranversales. */
		bool costs;
		/** Verdadero para mostrar la informacion de depuracion durante la busqueda. */
		bool debug;
		/** Numero de nodos expandidos. */
		unsigned int exp;
        /** Distancia recorrida por la ultima ruta calculada. */
        float distancia;
		/** Referencia a la informacion del terreno. */
		Map *map;
		/** Lista de puntos con la ruta obtenida en orden inverso (nodo objetivo el primero). */
		vector<POINT*> path;
		/** Lista de cerrados, indexada por el hash del nodo (sin duplicados). @see hash()*/
		std::map<unsigned int, POINT*> closed;
		/** Lista de abiertos, ordenada por F de menor a mayor (permite duplicados). */
		std::multimap<float, POINT*> open;
};

#endif //GSI_SEARCH
