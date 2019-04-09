#ifndef CDominio_H
#define CDominio_H

#include <ctype.h>
#include "CPDDLstruct.h"


/** @brief Almacena un vector con la informacion de las acciones.
 * 
 * Esta clase permite almacenar y acceder a un vector con la informacion de las acciones de que consta el dominio. El formato de las acciones esta definido por el tipo staccion y debera contener los datos necesarios para informar a la clase CPlan de los aspectos relativos al formateo de salida del planificador.
 */
class CDominio
{
    public:
        /** Constructor basico. Solo guarda la ruta al archivo de configuracion para su uso posterior.
         * @param configfile ruta al archivo de configuracion con los datos de las acciones.
         */
        CDominio(char* configfile);
        ~CDominio();

        /** Permite conocer el numero de acciones de que consta el dominio.
         * @return devuelve el numero de acciones almacenadas en el vector.
         */
        int numero_acciones_dominio();

        /** Devuelve la estructura completa de la accion ubicada en la posicion indicada.
         * @param indice debe estar comprendido entre 0 y el numero de acciones del dominio-1.
         * @param accion puntero a la estructura donde copiar la informacion.
         * @return devuelve 0 en caso de realizar la copia correctamente o -14 si el indice esta fuera de rango.
         */
        int accion_completa(int indice, staccion* accion);

        /** Devuelve la estructura completa de la accion cuyo nombre coincida con el nombre indicado.
         * @param accion nombre de la accion en PDDL a buscar.
         * @return devuelve el puntero a la estructura con los datos de la accion. En caso de no encontrase coincidencia devuelve NULL.
         */
        staccion* accion_completa(char* accion);

    protected:
        /** Inicia los datos necesarios de la clase. Para ello lee el fichero de configuracion hasta encontrar el indicador de numero de acciones CONFIG_ACCIONES y a partir de ahi lee el numero de acciones indicadas por la etiqueta.
         * @param rutadom ruta al archivo PDDL de dominio. <b>Para uso futuro</b>
         * @return Si ha leido el numero de ordenes necesarias devuelve 0. Si no puede abrir el fichero de configuracion devolvera -11, si es abierto pero no contiene acciones -12 y en caso de encontrar menos acciones de las esperadas -13.
         */
        int init(char* rutadom);

        /** Indica la posicion o token de la accion segun el formato del planificador.
         * @return devuelve el numero de token donde debe encontrarse el nombre de la accion. -14 si el vector esta vacio.
         * @note Este valor debe ser constante en todas las acciones ya que se asume que el formato es el mismo para todas las acciones. El primer token es el numero 1.
         */
        int token_nombre_accion();
        //Variables protegidas
        /** Numero de acciones que contiene el dominio. */
        int numacciones;
        /** Vector que contiene las estructuras con las acciones del dominio. */
        vector<staccion> acciones;
        
    private:
        /** Ruta al archivo de configuracion con la informacion de las acciones. */
        char rutaconfig[TAM_LINEA];
        /** Ruta al archivo con la definicion del dominio en PDDL. */
        char rutadominio[TAM_LINEA];
};

#endif // CDominio_H