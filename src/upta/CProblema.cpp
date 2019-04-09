#include "CProblema.h"

CProblema::CProblema(char* inicio, char* objetivos)
{
    if(inicio==NULL)
    {//Valores por defecto
        strcpy(inits,INIT);
        strcpy(metas,GOAL);
    }
    else
    {
        strcpy(inits,inicio);
        strcpy(metas,objetivos);
    }
}

CProblema::~CProblema()
{
}


void CProblema::init(char* rutaprob, char* rutaaux, char* inicio, char* objetivos)
{
    strcpy(rutaproblema,rutaprob);
    if(rutaaux!=NULL)
        strcpy(rutaauxiliar,rutaaux);
    else
    {
        strcpy(rutaauxiliar,rutaprob);
        strcat(rutaauxiliar,AUX);
    }
    if(inicio!=NULL)
    {
        strcpy(inits,inicio);
        strcpy(metas,objetivos);
    }
}


bool CProblema::objeto_pred(char* pred, int num)
{
    if(num<1) return false;
    char linea[TAM_LINEA];
    bool res;
    sprintf(linea,"(%s",pred);
    res=buscar((char*)linea);
    if(res)
    {
        char* temptok;
        int nobj=0;
        temptok=strtok(linea," ");
        while(temptok!=NULL && nobj!=num)
        {
            temptok=strtok(NULL," ");
            nobj++;
        }
        //Comprobar que es el objeto
        if(nobj!=num) return false;
        //Si es el ultimo objeto, eliminar el parentesis
        if(temptok[strlen(temptok)-1]==')')
            temptok[strlen(temptok)-1]='\0';
        strcpy(pred,temptok);
        return true;
    }
    else
        return false;
}


float CProblema::valor_funcion(char* pred, char* objs)
{
    char funcion[TAM_LINEA];
    if(objs==NULL || !strcmp(objs,(char*)"\0"))
        sprintf(funcion,"(= (%s)",pred);
    else
        sprintf(funcion,"(= (%s %s)",pred,objs);
    bool res;
    res=buscar((char*)funcion);
    if(res)
    {
        char* temptok;
        temptok=strtok(funcion,")");
        temptok=strtok(NULL,")");
        return atof(temptok);
    }
    else
        return 0;
}

int CProblema::insertar_objetos(char* objs)
{
	fstream entrada(rutaproblema,ios::in);
    fstream salida(rutaauxiliar,ios::out);

    if(!entrada.is_open() || !salida.is_open())
      //Archivos no abiertos
      return -1;
    //Archivos abiertos, iniciar copia
    int encontrado=0;
    encontrado=copiar_hasta((char*)OBJECTS,&entrada,&salida,true);
    //Previene incluir informacion al final del archivo
    if(encontrado==0)
    {
        //Intrudocir nuevo predicado
        char nuevo[TAM_LINEA];
        if(objs!=NULL)//Evitar espacios cuando no hay objetos
            sprintf(nuevo,"%s\n",objs);
        salida.write(nuevo,strlen(nuevo));
    }
    //Copiar el resto hasta el final
    copiar_hasta(NULL,&entrada,&salida,true);

    //Cerrar archivos
    entrada.close();
    salida.close();
    //Actualizar sistema archivos
    encontrado+=actualizar_archivos();
    return encontrado;
}

int CProblema::insertar_pred(char* pred, char* objs, bool meta)
{
    fstream entrada(rutaproblema,ios::in);
    fstream salida(rutaauxiliar,ios::out);

    if(!entrada.is_open() || !salida.is_open())
      //Archivos no abiertos
      return -1;
    //Archivos abiertos, iniciar copia
    int encontrado=0;
    if(!meta)
        encontrado=copiar_hasta(inits,&entrada,&salida,true);
    else
        encontrado=copiar_hasta(metas,&entrada,&salida,true);
    //Previene incluir informacion al final del archivo
    if(encontrado==0)
    {
        //Intrudocir nuevo predicado
        char nuevo[TAM_LINEA];
        if(objs!=NULL)//Evitar espacios cuando no hay objetos
            sprintf(nuevo,"(%s %s)\n",pred,objs);
        else
            sprintf(nuevo,"(%s)\n",pred);
        salida.write(nuevo,strlen(nuevo));
    }
    //Copiar el resto hasta el final
    copiar_hasta(NULL,&entrada,&salida,true);

    //Cerrar archivos
    entrada.close();
    salida.close();
    //Actualizar sistema archivos
    encontrado+=actualizar_archivos();
    return encontrado;
}


int CProblema::reemplazar_pred(char* pred, char* objsant, char* objsnew)
{
    fstream entrada(rutaproblema,ios::in);
    fstream salida(rutaauxiliar,ios::out);

    if(!entrada.is_open() || !salida.is_open())
      //Archivos no abiertos
      return -1;
    //Predicado a encontrar
    char aux[TAM_LINEA];
    if(objsant!=NULL)
        sprintf(aux,"(%s %s",pred,objsant);
    else
        sprintf(aux,"(%s",pred);
    //Iniciar copia hasta encontrar el predicado
    int encontrado=copiar_hasta(aux,&entrada,&salida,false);
    //Intrudocir nuevo predicado
    if(encontrado==0)
    {
        if(objsnew!=NULL)
            if(objsant==NULL)
                sprintf(aux,"(%s%s)\n",pred,objsnew);
            else
                sprintf(aux,"(%s %s)\n",pred,objsnew);
        else
            sprintf(aux,"(%s)\n",pred);
        salida.write(aux,strlen(aux));
    }
    //Copiar el resto hasta el final
    copiar_hasta(NULL,&entrada,&salida,true);

    //Cerrar archivos
    entrada.close();
    salida.close();
    //Actualizar sistema archivos
    encontrado+=actualizar_archivos();
    return encontrado;
}


int CProblema::insertar_funcion(char* pred, char* objs, float valor)
{
    char fun[TAM_LINEA];
    if(objs!=NULL)
        sprintf(fun,"= (%s %s)",pred,objs);
    else
        sprintf(fun,"= (%s)",pred);
    char dur[32];
    sprintf(dur,"%01.5f",valor);
    return insertar_pred(fun,dur);
}


int CProblema::reemplazar_funcion(char* pred, char* objs, float valor)
{
    char ant[TAM_LINEA];
    sprintf(ant,"= (%s",pred);
    char fun[TAM_LINEA];
    if(objs!=NULL)
        sprintf(fun,"%s) %01.3f",objs,valor);
    else
        sprintf(fun,") %f",valor);
    return reemplazar_pred(ant,objs,fun);
}


int CProblema::eliminar_predicado(char* pred, char* objs)
{
    fstream entrada(rutaproblema,ios::in);
    fstream salida(rutaauxiliar,ios::out);

    if(!entrada.is_open() || !salida.is_open())
      //Archivos no abiertos
      return -1;
    //Predicado a encontrar
    char aux[TAM_LINEA];
    if(objs!=NULL)
        sprintf(aux,"(%s %s",pred,objs);
    else
        sprintf(aux,"(%s",pred);
    //Iniciar copia hasta encontrar el predicado
    int encontrado=copiar_hasta(aux,&entrada,&salida,false);
    //Copiar el resto hasta el final (no copia el predicado buscado)
    copiar_hasta(NULL,&entrada,&salida,true);

    //Cerrar archivos
    entrada.close();
    salida.close();
    //Actualizar sistema archivos
    encontrado+=actualizar_archivos();
    return encontrado;
}


int CProblema::eliminar_funcion(char* pred, char* objs)
{
    char fun[TAM_LINEA];
    sprintf(fun,"= (%s",pred);
    return eliminar_predicado(fun,objs);
}


//FUNCIONES PRIVADAS *********
int CProblema::copiar_hasta(char* linea, fstream* origen, fstream* destino, bool copiarlinea)
{
    char buffer[TAM_LINEA];
    bool seguirbuscando=true;
    int chleidos=0;
    do{
        origen->getline(buffer,TAM_LINEA);
		chleidos=strlen(buffer);
        if(linea!=NULL && !strncmp(buffer,linea,strlen(linea)))
        {
            if(copiarlinea)
            {
                destino->write(buffer,chleidos);
                destino->write("\n",1);
            }
            seguirbuscando=false;
        }
        else
        {
            destino->write(buffer,chleidos);
            if(chleidos>0) destino->write("\n",1);
        }
    }while(chleidos>0 && seguirbuscando);

    //Si habia que buscar, comprobar que se ha encontrado
    if(linea!=NULL && seguirbuscando) return -2;
    return 0;
}


bool CProblema::buscar(char* linea)
{
    fstream entrada(rutaproblema,ios::in);
    char buffer[TAM_LINEA];
    bool seguirbuscando=true;
    do{
        entrada.getline(buffer,TAM_LINEA);
        if(buffer!=NULL && !strncmp(buffer,linea,strlen(linea)))
        {
            seguirbuscando=false;
            strcpy(linea,buffer);
        }
    }while(strlen(buffer)>0 && seguirbuscando);

    //Comprobar que se ha encontrado
    if(seguirbuscando) return false;
    else return true;
}


int CProblema::actualizar_archivos()
{
    //Eliminar original y cambiar nombre auxiliar
    int ret=remove(rutaproblema);
    if(ret!=0)
    {
        perror("\nFallo al borrar el archivo");
        return ret;
    }
    ret=rename(rutaauxiliar,rutaproblema);
    if(ret!=0)
    {
        perror("\nFallo al renombrar el archivo");
        return ret;
    }
    return 0;
}
