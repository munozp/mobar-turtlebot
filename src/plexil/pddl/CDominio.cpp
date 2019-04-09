#include "CDominio.h"

CDominio::CDominio(char* configfile)
{
    strcpy(rutaconfig,configfile);
}

CDominio::~CDominio()
{
}


int CDominio::init(char* rutadom)
{
    numacciones=0;
    strcpy(rutadominio,rutadom);

    //LEER FICHERO DE CONFIGURACION
    //Leer datos planificador
    fstream config(rutaconfig,ios::in);
    //Si se abre correctamente leemos
    if(config.is_open())
    {
    	char linea[TAM_LINEA]="";
    	while(!config.eof() && strncmp(linea,CONFIG_ACCIONES,strlen(CONFIG_ACCIONES)))
            config.getline(linea,TAM_LINEA,'\n');

        //Incio de acciones
        numacciones=atoi(strrchr(linea,' '));
        if(numacciones<=0) return -12; //No hay acciones
        else //leer acciones
        {
            int i=0;
            char* temptok;
            staccion nuevaacc;
            while(!config.eof() && i<numacciones)
            {
                config.getline(linea,TAM_LINEA,'\n');
                if(linea[0]!='\0' && linea[0]!=COMENTARIO)
                {
                    //Obtener informacion de la accion
                    temptok=strtok(linea," "); //Nombre de accion
                    strcpy(nuevaacc.nombre,temptok);
                    for(int j=0; j<3; j++) //Token posicion (accion, num objetos, duracion)
                    {
                        temptok=strtok(NULL," ");
                        nuevaacc.tokens[j]=atoi(temptok);
                    }
                    temptok=strtok(NULL," "); //Token de ruta PLEXIL
                    strcpy(nuevaacc.rutaPlexil,temptok);
                    //Insertar accion al final del vector
                    acciones.push_back(nuevaacc);
                    i++;
                }
            }
            if(i!=numacciones)
            {
                config.close();
                return -13; //No se ha leido el numero correcto de acciones
            }
        }
    }//if
    else //fallo al abrir
        return -11;

    config.close();
    return 0;
}


int CDominio::accion_completa(int indice, staccion* accion)
{
    if(indice>=0 && indice<numacciones)
    {
        strcpy(accion->nombre,acciones[indice].nombre);
        strcpy(accion->rutaPlexil,acciones[indice].rutaPlexil);
        for(int i=0; i<2; i++)
            accion->tokens[i]=acciones[indice].tokens[i];
        accion->duracion=acciones[indice].duracion;
        return 0;
    }
    else
        return -14;
}


staccion* CDominio::accion_completa(char* accion)
{
    staccion* temp=NULL;
    int i=0;
    while(temp==NULL && i<numacciones)
        if(!strncmp(accion,acciones[i].nombre,strlen(accion)))
            temp=&acciones[i];
        else
            i++;
    return temp;
}


int CDominio::token_nombre_accion()
{
    if(acciones.empty())
        return -14;
    else
        return acciones[0].tokens[0];
}


int CDominio::numero_acciones_dominio()
{
    return numacciones;
}