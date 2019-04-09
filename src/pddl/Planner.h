#ifndef Planner_H
#define Planner_H

#include <fcntl.h>
#include <sys/wait.h>
#include "CPDDLstruct.h"
#include "CProblema.h"
#include "CPlan.h"

/** @brief Esta clase instancia y controla todos los datos para un planificador.
 *
 * Permite instanciar un planificador PDDL generico y ejecutarlo de forma correcta mediante los datos contenidos en un fichero de configuracion. Permite la lectura del plan obtenido por este y la modificacion del archivo de problema.
 */
class Planner: public CProblema, public CPlan
{
    public:
        /** Instancia todos los elementos necesarios para el planificador a partir del fichero de configuracion.
         * @param configfile ruta al archivo de configuracion del planificador.
         */
        Planner(char* configfile);
        ~Planner();
        
        /** Permite actualizar los datos del planificador leyendo el archivo de configuracion dado.
         * @param configfile ruta al archivo de configuracion con los datos del planificador.
         * @return devuelve 0 si la lectura fue correcta o -31 si no pudo abrir el archivo y -32 si no ha encontrado todos los datos necesarios.
         */
        int actualizar_datos(char* configfile);

        /** Planifica o replanifica con los archivos PDDL indicados al instanciar la clase. Si se desea replanificar hay que modificar previamente los datos del problema (si procede).
         * @param replanificar si se establece a verdadero realiza la replanificacion. La unica diferencia con la planificacion inicial es que no inicia los datos para el dominio y el problema.
         * @return devuelve 0 si se realiza correctamente la planificacion o un valor negativo en caso contrario. Si el fallo es al crear el proceso para el planificador devolvera -33, en caso de fallo en el planificador dependera de cada planificador. Tambien propaga los errores de inicializacion de las clases CDominio y CPlan.
         */
        int planificar(bool replanificar=false);

        /** Lanza el planificador sin esperar por su finalizacion. Una vez finalize el planificador debe llamarse a inicializarEstructuras para completar el proceso de planificacion.
         * @return el PID del proceso que alberga el planificador. -33 en caso de fallo en el fork.
         */
        int planificarBG();

        /** Inicializa las estructuras PDDL tras la replanificacion. Vease planificar para los detalles de los parametros y valores de retorno.
         */
        int inicializarEstructuras(bool replanificar);

        /** Reinicia la lectura del plan generado. */
        int reiniciarPlan();

        /** Permite conocer el numero de veces que se ha replanificado para el planificador asociado.
         * @return el numero de veces que se ha replanificado. Sera -1 si solo se ha instanciado la clase y 0 si solo se ha planificado.
         */
        int num_replanificaciones();

    private:
        /** 0-planificador 1-parametro dominio 2-dominio 3-parametro problema 4-problema 5-parametro salida 6-salida 7-frase inicio acciones */
        char planificador[8][TAM_LINEA];
        /** Contador de replanificaciones. */
        int numreplan;
};

#endif // Planner_H
