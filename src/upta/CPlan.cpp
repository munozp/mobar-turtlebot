#include "CPlan.h"

CPlan::CPlan()
{
    numaccion=-1;
    numobjsultima=0;
}

CPlan::~CPlan()
{
	descplan.close();
}

int CPlan::init(char* rutaplan)
{
    numaccion=0;
    ultimastacc=NULL;
    tokenaccion=TOKEN_ACCION;
    char* cadinicio = (char*)INICIO_ACCIONES;
    //Abir el fichero de salida para lectura
    if(descplan.is_open()) //Reabrir
        descplan.close();
    descplan.open(rutaplan,ios::in);

    //Si se abre correctamente avanzamos hasta las acciones
    if(descplan.is_open())
    {
        char linea[TAM_LINEA];
        int cont=0;
        descplan.getline(linea,TAM_LINEA,'\n');//Lectura adelantada
        while(strncmp(linea,cadinicio,strlen(cadinicio)))
        {
            descplan.getline(linea,TAM_LINEA,'\n');
            cont++; //Para salir por un plan invalido
            if(cont>MAX_LINEA_ACCION) //No encuentra acciones
                return -22;
        }
        //Leer la linea en blanco
        descplan.getline(linea,TAM_LINEA,'\n');
        return 0; //Correcto
    }//if-Plan no abierto, fallo
    return -21;
}


int CPlan::sig_accion(char* accion, char* objetos, float* duracion)
{
    if(descplan.is_open())
    {
        //leer y analizar siguiente linea
        descplan.getline(ultimaaccion,TAM_LINEA,'\n');
        if(ultimaaccion[0]!='\0')
        {
            char* temptok;
            char aux[TAM_LINEA];
            strcpy(aux,ultimaaccion);
            //Eliminar no alfanumericos salvo '.', '-' y '_'
            unsigned int i;
            for(i=0; i<strlen(aux); i++)
                if(!isalnum(aux[i]) && aux[i]!='.' && aux[i]!='-' && aux[i]!='_') aux[i]=' ';
            i=1; //Numero de token
            unsigned int nobjs=0; //Numero de objetos
            unsigned int posdur=0; //Token de duracion
            temptok=strtok(aux," ");
            while(temptok != NULL)
            {
                if(i==tokenaccion)
                {
                    strcpy(accion,temptok); //Nombre de accion
                    i++; //Siguiente token
                    temptok=strtok(NULL," ");
                    //Buscar accion
                    if(!strcmp(accion, ACCION_DOCK) || !strcmp(accion, ACCION_UNDOCK))
  		    {
                        nobjs=NOBJS_DOCK;
                        posdur=TOKEN_ACCION+NOBJS_DOCK+1;
		    }
		    else if(!strcmp(accion, ACCION_PICT))
		    {
                        nobjs=NOBJS_PICT;
                        posdur=TOKEN_ACCION+NOBJS_PICT+1;
    		    }
		    else if(!strcmp(accion, ACCION_PTU))
		    {
                        nobjs=NOBJS_PTU;
                        posdur=TOKEN_ACCION+NOBJS_PTU+1;
  		    }
	            else
                    {
                        nobjs=NOBJS_MOVE;
                        posdur=TOKEN_ACCION+NOBJS_MOVE+1;
                    }
                    if(nobjs==0) //No hay objetos
                        strcpy(objetos,"");
                    else
                        for(unsigned int j=0; j<nobjs; j++) //Leer objetos
                        {
                            if(j==0) //Limpiar parametro y poner primer objeto
                                sprintf(objetos,"%s",temptok);
                            else //Añadir objetos siguientes
                                sprintf(objetos,"%s %s",objetos,temptok);
                            i++;//Siguiente token
                            temptok=strtok(NULL," ");
                        }//for
                        strcpy(objetosultima,objetos);
                        numobjsultima=nobjs;
                }//if accion y objetos
                else
                    if(i==posdur) //Duracion
                    {
                        *duracion=atof(temptok);
                        i++; //Siguiente token
                        temptok=strtok(NULL," ");
                    }
                    else//Token no necesario, siguiente
                    {
                        i++; //Siguiente token
                        temptok=strtok(NULL," ");
                    }
            }//while fin accion
            numaccion++;
            return numaccion;
        }//if hay acciones
        else
        {//Definir valores del final del plan
            strcpy(accion,FINAL_PLAN);
            strcpy(objetos,"\0");
            strcpy(objetosultima,"\0");
            *duracion=-1;
            numobjsultima=0;
            numaccion=0;
            return numaccion; //Final de plan
        }
    }//if inicial
    else //Plan no abierto
    {
        strcpy(accion,PLAN_NO_OPEN);
        strcpy(objetos,"\0");
        strcpy(objetosultima,"\0");
        *duracion=-1;
        numobjsultima=0;
        return -21;
    }
}


int CPlan::sig_accion(char* accion, char** objetos, float* duracion, int* numobjetos)
{
    char* temp=new char[TAM_LINEA];
    int aux=sig_accion(accion,temp,duracion);
    delete[] temp;
    if(aux>0)
        *numobjetos=lista_objetos(objetos);
    else
        *numobjetos=0;
    return aux;
}


int CPlan::lista_objetos(char** objetos)
{
    //Eliminar contenido previo y reservar memoria para el numero de objetos
  //  if(objetos != NULL)
  //      delete[] (char*)objetos;
  //  objetos = new char*[numobjsultima];
    char* aux;
    aux=strtok(objetosultima," ");
    int i=0;
    while(aux!=NULL)
    {
        objetos[i]=new char[strlen(aux)];
        strcpy(objetos[i],(char*)aux);
        aux=strtok(NULL," ");
        i++;
    }
    return i;
}


char* CPlan::cadena_accion_proceso()
{
    return (char*)ultimaaccion;
}

int CPlan::numero_accion_proceso()
{
    return numaccion;
}

int CPlan::numero_objetos_proceso()
{
    return numobjsultima;
}
