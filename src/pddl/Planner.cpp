#include "Planner.h"

Planner::Planner(char* configfile):
CProblema(),
CPlan(configfile)
{
    numreplan=-1;
    actualizar_datos(configfile);
    CProblema::init(planificador[4]);
}


Planner::~Planner()
{
}


int Planner::actualizar_datos(char* configfile)
{
    //LEER FICHERO DE CONFIGURACION
    //Leer datos planificador
    fstream config(configfile,ios::in);
    //Si se abre correctamente leemos
    if(config.is_open())
    {
    	char linea[TAM_LINEA];
    	int numdato=0;
    	while(numdato<8 && !config.eof())
    	{
            config.getline(linea,TAM_LINEA,'\n');
            if(linea[0]!=COMENTARIO)
            {
                strcpy(planificador[numdato],linea);
                numdato++;
            }
        }
        if(numdato<8) return -32; //No se ha leido todo lo necesario
    }//if
    else //fallo al abrir
        return -31;

    config.close();
    return 0;
}


int Planner::planificar(bool replanificar)
{
    int estadosal=0;
    char cmdplanner[2056];
    sprintf(cmdplanner,"%s %s %s %s %s %s %s\n",planificador[0],planificador[1],planificador[2],planificador[3],planificador[4],planificador[5],planificador[6]);
    // Planificar
    estadosal = system(cmdplanner);
    //COMPROBAR VALOR DE SALIDA DE LA ORDEN
    if(estadosal==0)
        return inicializarEstructuras(replanificar);
    else
        return estadosal; //Fallo al ejecutar el hijo
}

int Planner::planificarBG()
{
    int PIDplanificador=fork();
    if(PIDplanificador==-1) return -33; //error en fork

    if(PIDplanificador)
        return PIDplanificador;
    else //Hijo, ejecucion del planificador
        execl(planificador[0],planificador[0],planificador[1],planificador[2],planificador[3],planificador[4],planificador[5],planificador[6],NULL);
}

int Planner::inicializarEstructuras(bool replanificar)
{
    int ret=0;
    //Inicializar los datos de las clases heredadas
    if(!replanificar)
    {
        numreplan=0;
        //Reiniciar CDominio y CProblema
        ret=CDominio::init(planificador[2]);
        if(ret==0)
            CProblema::init(planificador[4]);
        else
            return ret;
    }
    ret=CPlan::init(planificador[6],planificador[7]);
    numreplan++;
    return ret;
}

int Planner::reiniciarPlan()
{
    return CPlan::init(planificador[6],planificador[7]);
}

int Planner::num_replanificaciones()
{
    return numreplan;
}
