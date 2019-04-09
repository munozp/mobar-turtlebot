#ifndef CProblema_H
#define CProblema_H

#include "CPDDLstruct.h"


/** Linea que indica la definicion de estados iniciales. */
#define INIT "(:init"
/** Linea que indica la definicion de las metas. */
#define GOAL "(:goal"
/** Linea que indica la definicion de los objetivos. */
#define OBJECTS "(:objects"
/** Terminacion por defecto para el archivo auxiliar de copia. */
#define AUX "aux"

/** @brief Clase que permite modificar el archivo del problema PDDL.
 *
 * Esta clase permite insertar o modificar estados iniciales, funciones o metas del problema PDDL empleado para la planificacion. Para ello el archivo debe tener una unica linea por predicado sin espacios antes ni despues del mismo. Actualmente no se permite insertar o modificar los objetos, por tanto no se podran hacer modificaciones que incluyan nuevos objetos o la planificacion fallara.
 * Para actualizar el problema se realiza una copia de este en un fichero auxiliar, que sera renombrado como el problema original tras haber eliminado la version anterior.
 * @note Para todas las funciones de la clase se deberan indicar los parametros sin espacios al inicio/fin del predicado/objetos y sin caracteres de puntuacion (parentesis signo '=' para funciones, etc.).
 */
class CProblema
{
    public:
        /** Constructor de la clase, inicializa las lineas de inicio de estados iniciales y metas del fichero PDDL. Si no se indican, toma valores por defecto (:init y (:goal(and.
         * @param inicio cadena de inicio de los estados iniciales.
         * @param objetivos cadena de inicio de las metas del problema PDDL.
        */
        CProblema(char* inicio=NULL, char* objetivos=NULL);
        ~CProblema();

        /** Devuelve el objeto pedido de un predicado del problema (tanto de estado incial como de metas).
         * @param pred nombre del predicado a buscar. Se sustituira por el objeto encontrado.
         * @param num numero del objeto del predicado deseado. Debe ser mayor que 0 (primer objeto = 1).
         * @return devuelve verdadero si ha obtenido el objeto o falso en caso contrario. El objeto encontrado se copiara en la direccion apuntada por pred.
         */
        bool objeto_pred(char* pred, int num);

        /** Devuelve el valor de una funcion.
         * @param pred nombre de la funcion a buscar.
         * @param objs objetos de la funcion a buscar. Si la funcion no tiene objetos debera ser NULL y devolvera el valor de la primera funcion coincidente.
         * @return devuelve el valor de la funcion encontrada o 0 en caso de no encontrarla.
         */
        float valor_funcion(char* pred, char* objs);

		/**
		*/
		int insertar_objetos(char* objs);

        /** Inserta un nuevo estado o meta en el problema.
         * @param pred nombre del predicado a insertar.
         * @param objs objetos tal y como seran insertados, con un espacio entre objeto y objeto. NULL si no hay objetos.
         * @param meta indica que el predicado es una nueva meta.
         * @return devuelve 0 si se ha insertado correctamente y -1 en caso contrario.
         */
        int insertar_pred(char* pred, char* objs, bool meta=false);

        /** Cambia un estado o meta del problema por otro nuevo. Para ello el estado o meta debera existir previamente en el archivo de problema.
         * @param pred nombre del predicado nuevo y antiguo (debe coincidir).
         * @param objsant lista de objetos separados por espacios del predicado a sustituir. NULL si no hay objetos.
         * @param objsnew lista de objetos nuevos. NULL si no hay objetos.
         * @return 0 si se sustituye correctamente, -2 si falla por no encontrar el anterior predicado y -1 por cualquier otro fallo.
         */
        int reemplazar_pred(char* pred, char* objsant, char* objsnew);

        /** Inserta una nueva funcion en el problema.
         * @param pred nombre de la funcion a insertar.
         * @param objs objetos tal y como seran insertados, con un espacio entre objeto y objeto.  NULL si no hay objetos.
         * @param valor valor de la funcion a insertar.
         * @return devuelve 0 si se ha insertado correctamente y -1 en caso contrario.
         */
        int insertar_funcion(char* pred, char* objs, float valor);

        /** Cambia el valor de una funcion del problema. Para ello la funcion debera existir previamente en el archivo de problema.
         * @param pred nombre de la funcion nueva y antigua (debe coincidir).
         * @param objs lista de objetos separados por espacios de la funcion. NULL si no hay objetos.
         * @param valor nuevo valor de la funcion.
         * @return 0 si se sustituye correctamente, -2 si falla por no encontrar la funcion y -1 por cualquier otro fallo.
         */
        int reemplazar_funcion(char* pred, char* objs, float valor);

        /** Elimina un predicado del problema.
         * @param pred predicado a eliminar.
         * @param objs objetos del predicado. NULL si no tiene objetos.
         * @return 0 si se elimina correctamente -2 si no se encuentra y -1 si falla por cualquier otro motivo.
         */
        int eliminar_predicado(char* pred, char* objs);

        /** Elimina una funcion del problema.
         * @param pred predicado de la funcion a eliminar a eliminar.
         * @param objs objetos del predicado. NULL si no tiene objetos.
         * @return 0 si se elimina correctamente -2 si no encuentra la funcion y -1 si falla por otro motivo.
         */
        int eliminar_funcion(char* pred, char* objs);

        /** Permite establecer la ruta al archivo de problema y actualizar las lineas de inicio de datos.
         * @param rutaprob ruta al archivo de problema PDDL.
         * @param rutaaux ruta al archivo de copia auxiliar. Si no se establece sera rutaproblema+AUX.
         * @param inicio cadena de inicio de los estados iniciales. Si es NULL no se actualizan las lineas.
         * @param objetivos cadena de inicio de las metas del problema PDDL.
         */
        void init(char* rutaprob, char* rutaaux=NULL, char* inicio=NULL, char* objetivos=NULL);

    //Funciones privadas
    private:
        /** Copia desde archivo origen a destino hasta que encuentra la linea especificada en el flujo origen y deja el puntero en la linea siguiente a la posicion de dicha linea.
         * @param linea linea a buscar. Si se indica una cadena vacia (NULL) copia hasta el final del archivo.
         * @param origen flujo del archivo en el que buscar y origen de datos.
         * @param destino flujo donde se copiaran los datos hasta encontrar linea que no sera copiada.
         * @param copiarlinea si es verdadero indica que la linea encontrada tambien tiene que ser copiada.
         * @return devuelve 0 si todo es correcto o u otro valor en caso contrario.
         */
        int copiar_hasta(char* linea, fstream* origen, fstream* destino, bool copiarlinea);

        /** Busca un predicado o funcion dentro del archivo de problema.
         * @param linea nombre del predicado o funcion a buscar. En caso de un predicado sera solo el nombre del predicado, para una funcion debera incluir, ademas, los objetos de dicha funcion. En caso de encontrar la linea deseada, devolvera la linea completa del archivo de problema.
         * @return devuelve verdadero si encuentra la informacion buscada o falso en caso contrario.
         */
        bool buscar(char* linea);

        /** Elimina el archivo original del problema y lo reemplaza por el modificado.
         * @return Devuelve 0 si fue todo correcto u otro valor en caso contrario.
         */
        int actualizar_archivos();

    private:
        /** Ruta al archivo de problema PDDL. */
        char rutaproblema[TAM_LINEA];
        /** Ruta al archivo auxiliar de copia. */
        char rutaauxiliar[TAM_LINEA];
        /** Linea de inicio de los estados en el archivo. */
        char inits[TAM_LINEA];
        /** Linea de inicio de las metas en el archivo. */
        char metas[TAM_LINEA];
        //fstream descprob;
};

#endif // CProblema_H

