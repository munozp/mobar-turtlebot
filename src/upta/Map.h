#ifndef MAP_H
#define MAP_H

#include "common_defs.h"

/**
 * Informacion del terreno y estructuras relacionadas.
 */
class Map {

	public:
		/**
         * Constructor (no realiza ninguna accion).
         */
		Map();
		/**
         * Destructor (no realiza ninguna accion).
         */
		~Map();
		/**
         * Carga la informacion del mapa a partir de un archivo.
		 * @param filename ruta al archivo con el mapa
		 * @return verdadero si el mapa se ha cargado correctamente, falso en otro caso
                */
		bool read_map(const char* filename);
        /**
         * Limpia los datos de G y H de los nodos del mapa.
         */
		void reset_data();
		/**
		 * Crea el margen de seguridad alrededor de los obstaculos.
		 * @param desp desplazamiento en casillas para crear el margen de seguridad (minimo 1).
		 * @param segf factor en que se amplia el valor de seguridad por cada casilla (mayor que 0.1).
		 * @return verdadero al finalizar, falso en caso de que los argumentos no sean validos.
		 */
		bool safety_margin(int desp, float segf);
        void deactivate_safety_margin();
		/**
         * Crea un nuevo mapa de interiores a partir de la combinacion aleatoria de los patrones ubicados 
		 * en /test/ con nombres r0-r6. Estos deben ser mapas cuadrados (dimension NxN) y todos de la misma dimension.
		 * @param filename archivo donde se guardara el mapa generado
		 * @param ncols numero de patrones a poner en horizontal (eje X)
		 * @param nrows numero de patrones a poner en vertical (eje Y)
		 * @param maxc dimension de los patrones empleados (NxN)
		 * @return verdadero si el mapa se ha generado y guardado correctamente, falso en otro caso
         */
		bool write_map(const char* filename, int ncols, int nrows, int maxc);
		/**
		 * Crea un archivo con la solucion encontrada.
		 * @param filename ruta al archivo donde se guardara la solucion
		 * @param path vector con la lista de puntos que contiene la solucion en orden inverso (primero goal)
		 * @param res resolucion del mapa en decimetros.
		 * @param dat cadena de texto con datos auxiliares a incluir en el archivo como cabecera.
		 * @return verdadero se se ha realizado la operacion correctamente. Falso en caso contrario.
		 */
		bool write_path(const char* filename, vector<POINT*> path, float res, const char* dat);
		/**
		 * Muestra por pantalla el mapa completo. Los obstaculos se muestran usando el caracter OBS. @see OBS
		 * @param withc si es verdadero muestra el coste de cada celda, sino muestra un espacio en blanco
		 */
		void print_map(bool withc = false);

		/**
		 * Devuelve la referencia al nodo con coordenadas (x,y).
		 * @param x coordenada x del nodo solicitado
		 * @param y coordenada y del nodo solicitado
		 * @return el puntero al nodo solicitado. Si las coordenadas no fueran validas
		 * devolveria la referencia a un nodo vacio. @see void_p
		 */
		POINT* get_point(int x, int y);
		/**
		 * Obtiene una lista con todos los sucesores del nodo p que sean alcanzables 
		 * (es decir, no sean un obstaculo).
		 * @param p nodo del que obtener sus sucesores
		 * @return un vector con los punteros a los sucesores del nodo p
		 */
		vector<POINT*> get_succesors(const POINT* p);
		/**
		 * Calcula la distancia euclidea entre dos nodos.
		 * @param p un nodo
		 * @param t otro nodo
		 * @return un valor mayor o igual a 0 con la distancia en linea recta entre los nodos
		 */
		float get_dist(const POINT* p, const POINT* t);
		/**
		 * Calcula la distancia euclidea entre dos puntos dados por sus coordenadas.
		 * @param x0 coordenada x del primer punto
		 * @param y0 coordenada y del primer punto
		 * @param x1 coordenada x del segundo punto
		 * @param y1 coordenada y del segundo punto
		 * @return un valor mayor o igual a 0 con la distancia en linea recta entre los dos puntos
		 */
		float get_dist(int x0, int y0, int x1, int y1);
		/**
		 * Comprueba si un nodo dado existe en el mapa.
		 * @param p nodo a comprobar
		 * @return verdadero si las coordenadas x e y del nodo pertenecen al mapa
		 */
		bool valid_pos(const POINT* p);
		/**
		 * Comprueba si un punto dado por sus coordenadas existe en el mapa.
		 * @param x coordenada x a comprobar
		 * @param y coordenada y a comprobar
		 * @return verdadero si las coordenadas x e y dadas pertenecen al mapa
		 */
		bool valid_pos(int x, int y);
		/**
		 * Comprueba si un nodo dado es un obstaculo.
		 * @param p nodo a comprobar
		 * @return verdadero si el nodo p es un obstaculo o no pertenece al mapa
		 */
		bool is_obstacle(const POINT* p);
		/**
		 * Comprueba si un punto dado por sus coordenadas es un obstaculo.
		 * @param x coordenada x a comprobar
		 * @param y coordenada y a comprobar
		 * @return verdadero si las coordenadas x e y corresponden a un obstaculo o estan fuera del mapa
		 */
		bool is_obstacle(int x, int y);
		/**
		 * @return el numero de columnas del mapa
		 */
		int get_ncols();
		/**
		 * @return el numero de filas del mapa
		 */
		int get_nrows();

	private:
		/** Numero de filas del mapa. */
		int cols;
		/** Numero de columnas del mapa. */
		int rows;
		/** Valor a partir del cual se considera que una posicion es un obstaculo (no incluido). */
		static int MAX_C;
		/** Caracter con el que se muestran los obstaculos al mostrar el mapa por pantalla. */
		static char OBS;
		/** Nodo vacio (se devuelve al intentar acceder a un nodo invalido). */
		static POINT void_p;
		/** Estructura con toda la informacion del mapa. Acceso a un punto: map[fila/y][columna/x]. */
		vector<vector<POINT> > map;
};

#endif //MAP
