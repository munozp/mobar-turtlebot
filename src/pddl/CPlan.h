#ifndef CPlan_H
#define CPlan_H

#include "CPDDLstruct.h"
#include "CDominio.h"


/** @brief Clase para la lectura del archivo de salida del planificador.
 *
 * Esta clase permite leer el plan generado por el planificador a partir del archivo en que haya sido guardado. Para ello requiere conocer el formato de salida de las ordenes y la ruta al archivo.
 * @note Esta clase asume que la salida del planificador es del estilo <i>datos <b>accion obj1 obj2... </b> datos <b>duracion</b> datos</i> pudiendo variar los campos <i>datos</i> en forma y posicion, y <i>duracion</i> puede estar tanto antes como despues de la accion, pero los objetos siempre iran precedidos del nombre de la accion. La posicion de cada elemento vendra dada por los datos de CDominio.
*/
class CPlan: public CDominio
{
    public:
        /** Constructor de la clase, no realiza ninguna accion.
         * @param configfile ruta al archivo de configuracion con la descripcion de las acciones.
         */
        CPlan(char* configfile);
        ~CPlan();

        /** Funcion que lee la siguiente accion del plan. Los datos se devolveran en los parametros por referencia. Si se ha llegado al final devuelve FINAL_PLAN y duracion -1.
         * @param accion nombre de la accion leida.
         * @param objetos lista de parametros de la accion, separados por espacios. Si no hay objetos devuelve cadena vacia.
         * @param duracion duracion de la accion, 0 en caso de no tener.
         * @return devuelve el numero de secuencia de la accion (comenzando en 1) o 0 al llegar al final. Si fallase la apertuar del archivo del plan devolveria -21, si no hubiese acciones -22 y si encontrase una accion desconcida -23.
         */
        int sig_accion(char* accion, char* objetos, float* duracion);

        /** Funcion sig_accion sobrecargada para permitir obtener la siguiente accion y la lista completa de parametros de acuerdo al resultado de la funcion lista_objetos en una unica llamada.
         * @param accion nombre de la accion leida.
         * @param objetos puntero a la matriz de caracteres donde almacenar el objeto.
         * @param duracion duracion de la accion, 0 en caso de no tener.
         * @param numobjetos numero de objetos de la ultima accion leida.
         * @return devuelve el numero de secuencia de la accion o -1 al llegar al final. Si el archivo del plan no estuviese abierto devolveria -2. En caso de que la accion no haya sido declarada en el archivo de configuracion o la declaracion sea incorrecta, al no encontrar la accion leida devolveria -3.
         * @note la funcion reserva la memoria necesaria para almacenar la lista de objetos.
         * @warning <b>funcion no probada, no se recomienda su uso.</b>
         */
        int sig_accion(char* accion, char** objetos, float* duracion, int* numobjetos);

        /** Devuelve una lista con los objetos de la ultima accion leida.
         * @param objetos puntero a la matriz de caracteres donde almacenar el objeto.
         * @return devuelve el numero de objetos de la accion.
         * @note la funcion reserva la memoria necesaria para almacenar la lista de objetos.
         * @warning <b>funcion no probada, no se recomienda su uso.</b>
         */
        int lista_objetos(char** objetos);

        /** Permite conocer el nombre de conexion entre la accion PDDL y la accion a ejecutar por otra capa de la arquitectura, definido en el archivo de configuracion.
         * @return devuelve el nombre del conector para la ultima accion leida.
         */
        char* conector_accion();

        /** Permite obtener la cadena de la ultima accion leida tal y como es obtenida del planificador.
         * @return la cadena con la ultima accion leida del fichero del plan.
         */
        char* cadena_accion_proceso();

        /**@return devuelve el numero de secuencia de la ultima orden leida. */
        int numero_accion_proceso();

        /**@return devuelve el numero de objetos de la ultima orden leida. */
        int numero_objetos_proceso();

    protected:
        /** Esta funcion inicia todos los datos necesarios para leer el plan. Abre (o reabre si estaba abierto) el archivo de salida del planificador especificado y leera dicho fichero hasta el inicio de las ordenes, marcado por la cadena de inicio de las ordenes seguido de una linea en blanco.
         * @param rutaplan ruta al archivo de salida del planificador.
         * @param cadinicio cadena que marca el inicio de las ordenes (tras encontrar esta cadena se lee una linea extra).
         * @return devuelve 0 si todo ha ido correcto. -21 si no puede abrir el archivo o -22 si el archivo no contiene acciones.
         */
        int init(char* rutaplan, char* cadinicio);

    private:
        /** Datos del archivo de salida del planificador. */
        fstream descplan;
        /** Posicion (token) donde se encuentra el nombre de la accion en el fichero de salida. */
        int tokenaccion;
        /** Ultima accion leida tal y como es generada por el planificador. */
        char ultimaaccion[TAM_LINEA];
        /** Componentes de la ultima accion leida. */
        staccion* ultimastacc;
        /** Cadena con los objetos de la ultima accion. */
        char objetosultima[TAM_LINEA-MAX_CAR];
        /** Numero de secuencia de la ultima accion leida. */
        int numaccion;
        /** Numero de objetos de la ultima accion leida. */
        int numobjsultima;
};

#endif // CPlan_H
