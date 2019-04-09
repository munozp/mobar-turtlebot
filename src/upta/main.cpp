/** 
 * File:   main.cpp
 * Integracion task-planning y path-planning
 */

#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "common_defs.h"
#include "Map.h"
#include "Heuristica.h"
#include "GSIsearch.h"
#include "CPDDLstruct.h"
#include "CProblema.h"
#include "CPlan.h"
#include <cfloat>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace std;

/** Mostrar informacion de depuracion (ON si !=0). */
#define DEBUG 0
/** Resolucion del mapa. */
float RES = 1;
/** Factor multiplicativo para podar busqueda si la heuristica es mayor que LOC_MAX*H_FACTOR. */
#define H_FACTOR 2
#define LOC_MAX 100
/** Factor divisor para evitar localizarse si la distancia recorrida no es mayor que LOC_MAX/LOC_MIN_FACTOR. */
#define LOC_MIN_FACTOR 2
/** Minima distancia en dm para iniciar solicitud de localizacion. */
#define LOC_MIN 70
/** Para marcar coordenadas inalcanzables entre si. */
#define NO_ALCANZABLE -2
// Al cambiar planificador cambiar INICIO_ACCIONES  en CPDDLstruct.h
/** Ruta al ejecutable del planificador de tareas. */
#define TASK_PLANNER "/home/gsi/Escritorio/mobar/bin/planner/optic-clp"
//#define TASK_PLANNER "/home/gsi/Escritorio/mobar/bin/planner/sgplan"
/** Parametro para definir el dominio. */
#define TASK_DOM "-T -E -N"
//#define TASK_DOM "-o"
/** Parametro para definir el problema. */
#define TASK_PROB ""
//#define TASK_PROB "-f"
/** Parametro para definir la salida del task-planner. */
#define TASK_OUT ">"
//#define TASK_OUT "-out"
/** Archivo de salida del planificador. */
#define OUT_TASK_PLAN "taskplan"
//#define OUT_TASK_PLAN "taskplan"
/** Tamaño de una coordenada en caracteres. */
#define T_PUNTO 150


/**
 * Retorna el punto en formato Cxx_yy.
 * @param n nodo a convertir en cadena.
 * @param str cadena de caracteres donde alojar la conversion.
 */
void getCelda(const POINT &n, char *str)
{
    sprintf(str, "c%d%s%d", n.X, POS_DELIMITER, n.Y);
}
void getCelda(const int x, const int y, char* str)
{
	POINT p(x,y,1);
	getCelda(p, str);
}

/**
 * Obtiene las coordenadas X e Y a partir de Cxx_yy.
 * @param lin cadena con la coordenada en formato Cxx_yy. Sera modificada.
 * @param x coordenada x obtenida.
 * @param y coordenada y obtenida.
 */
bool getCoords(char *lin, int *x, int *y)
{
    char* temptok;
    lin[0] = ' '; // Eliminar 'C'
    temptok = strtok(lin,(char*)POS_DELIMITER); // Primer token, X
    if(temptok != NULL)
    {
        *x = atoi(temptok);
        temptok = strtok(NULL,(char*)POS_DELIMITER); // Segundo token, Y
        if(temptok != NULL){
            *y = atoi(temptok);
			if (DEBUG!=0) std::cerr << "[" << *x << ", " << *y <<"]"<< endl;
			return true;
		}
		else return false;
    }else return false;
}


/**
 * Obtener las coordenadas alojadas en un fichero.
 * @param ruta ubicacion del archivo.
 * @param puntos vector donde almacenar las coordenadas leidas.
 * @return -1 si no hay archivo, 0 si no se ha leido nada o el numero de coordenadas leidas.
 */
int obtener_coordenadas(const char* ruta, vector<POINT> *puntos)
{
	// Archivo con las coordenadas
	fstream file;
	file.open(ruta, std::ios::in);
	if(!file.is_open())
	{
		std::cerr << "No se ha podido abrir el archivo de entrada " << ruta << endl;
		return -1;
	}
	int numleidos = 0;
	// Leer el archivo de entrada linea a linea
	if (DEBUG!=0) std::cerr << "Leyendo coordenadas de " << ruta << endl;
	char lin[T_PUNTO];
	char tmp[T_PUNTO];
	int x,y;
	bool duplicado = false;
	file.getline(lin, sizeof(lin), '\n'); // Lectura adelantada
	while(!file.eof()){
		for (int i=0;i<=sizeof(lin)-1;i++){
			if (lin[i]=='c' && i<sizeof(lin)-1 && isdigit(lin[i+1])){
				if(DEBUG) std::cout<<lin<<std::endl;
				for (int j=i;j<=sizeof(lin)-1;j++){
						if (lin[j]!=' ' && lin[j]!=')' && lin[j]!='=')				
							tmp[j-i]=lin[j];
						else break;
				}
				if (strcmp(tmp,"")>0){
					if (getCoords(tmp,&x,&y)){
						duplicado = false;
						//Comprueba duplicados
						for (int i=0;i<puntos->size();i++){
							if ((* puntos)[i].X == x && (*puntos)[i].Y == y) duplicado = true;
						}
						if (!duplicado){
							puntos->push_back(POINT(x,y,1));
							numleidos++;
						}		
					}
				}
				memset(tmp,'\0',sizeof(tmp)); // Vaciar cadena
				
			}
		}
		memset(lin,'\0',sizeof(lin)); // Vaciar cadena
		file.getline(lin, sizeof(lin), '\n');
	}
	if (DEBUG!=0) std::cerr << "Leidos: " << numleidos << endl;
	return numleidos;
}

/**
 * Compara flotantes para el quick sort.
 */
int compareFloats(const void *a, const void *b)
{
    return ( *(float*)a - *(float*)b );
}


/**
 * Convierte una cadena de caracteres a minuscula.
 * @param str la cadena a convertir a minusculas.
 */
void toLowercase(char* str)
{
    for(int i = 0; str[i]; i++) 
        str[i] = tolower(str[i]);
}

/**
 * Ejecuta el taskplanner con los archivos indicados.
 * @param dominio ruta al archivo que contiene el dominio PDDL.
 * @param problema rutah al archivo con estado inicial y metas.
 * @return Devuelve true si se ha ejecutado correctamente y ha obtenido un plan. Falso en cualquier otro caso.
 */
bool lanzar_taskplanner(const char* dominio, const char* problema)
{
    int estadosal = 0;
    char cmdplanner[2056];
    sprintf(cmdplanner, "%s %s %s %s %s %s %s" ,TASK_PLANNER,TASK_DOM,dominio,TASK_PROB,problema,TASK_OUT,OUT_TASK_PLAN);
    if(DEBUG) std::cout<<"Executing planner: "<<cmdplanner<<std::endl;
    estadosal = system(cmdplanner);

    //COMPROBAR VALOR DE SALIDA DE LA ORDEN
    if(estadosal==0)
        return true;    
    else
        return false; //Fallo al ejecutar el hijo
}


/**
 * Manejador para el consumo excesivo de memoria
 */
void SignalHandler(int signal)
{
    std::cerr << "Out of memory" << endl;
    throw "";
}

bool printCoords(const char *ruta, vector<POINT> *puntos) {
	
	char bufferOut[250];
	char tmp[32];

	for (int i=0;i<puntos->size();i++){
		getCelda((*puntos)[i],tmp);
		strcat(bufferOut,tmp);
		strcat(bufferOut,(char*)" ");
		}
	strcat(bufferOut," - loc");

	CProblema aux;
	aux.init((char*)ruta);
	aux.insertar_objetos(bufferOut);

	return true;
}


/**
 * Funcion principal.
 * Los argumentos deben ser: dominio problema -t[c] mapa resolucion margen seguridad
 * Donde la resolucion y el margen de seguridad son valores en dm.
 */
int main(int argc, char** argv) 
{
	struct timeval t_ini, t_fin;
	gettimeofday(&t_ini, NULL);
	Map map;
	float res, desp;
	bool costs = false;

	if(argc != 9)
	{	
		std::cerr << "Numero incorrecto de argumentos\n";
		std::cerr << " Debe ser: -o dominio -f problema -t[c] mapa <resolucion> <margen seguridad>\n";
		std::cerr << " Donde la resolucion y el margen de seguridad son valores en dm\n";
		return -1;
	}
	else
	{
        // Manejador para controlar out of memory
        typedef void (*SignalHandlerPointer)(int);
        SignalHandlerPointer previousHandler;
        previousHandler = signal(SIGSEGV , SignalHandler);

        // Cargar mapa de planta
		if(!map.read_map(argv[6]))
		{
			std::cerr << "Fallo al leer el archivo de mapa " << argv[4] << endl;
			return -1;
		}
		// Leer y crear margen de seguridad
		res = atof(argv[7]);
		std::cerr << "Resolucion del mapa: " << res << " dm." << endl;
		    RES = res;
		desp = atof(argv[8]);
		std::cerr << "Estableciendo margen de seguridad... ";
		if(map.safety_margin((int)(ceil(desp/res)), desp))
			std::cerr << desp << " dm." << endl;
		else
			std::cerr << "no establecido." << endl;
		if(sizeof(argv[5])>2 && argv[5][2] == 'c')
		{
			costs = true;
			std::cerr << "Usando costes transversales." << endl;
		}
    }
    // Obtener el vector de coordenadas de interes
    vector<POINT> metas;
    vector<POINT> balizas;
    int nmetas = obtener_coordenadas(argv[4], &metas);
    if(nmetas < 0)
        return -1;
	if (!printCoords(argv[4], &metas))
		return -1;
    // Array con las distancias entre todos los puntos de interes
    int totalPuntos = metas.size();
    double distancias[totalPuntos][totalPuntos];
    double TSPdistancias[totalPuntos][totalPuntos];
    for(int i=0; i<totalPuntos; i++)
     for(int j=0; j<totalPuntos; j++)     
         if(i == j)
         {
             distancias[i][j] = NO_ALCANZABLE;
             TSPdistancias[i][j] = NO_ALCANZABLE;
         }
         else
         {
             distancias[i][j] = -1;
             TSPdistancias[i][j] = -1;
         }
    // Crear el path planner
	POINT inicio,fin;
	Heuristica h_euclidea(Heuristica::EUCLIDEA);
	GSIsearch theta(&map, costs, DEBUG!=0);

    // Calcular distancias entre posicion inicial, metas y recargas
    // Las distancias entre dos recargas no se calculan
    CProblema problema;
    problema.init(argv[4]);
    bool hayruta = false;
    float dist = 0;
    for(int i = 0; i<nmetas; i++)
		for(int j = i+1; j<nmetas; j++)
		{
		    hayruta = false;
		    // La distancia entre metas se calcula si la distancia es menor que H_FACTOR*LOC_MAX
		    if(i == 0 || (!map.is_obstacle(&metas[i]) && !map.is_obstacle(&metas[j])
		               && h_euclidea.getH(&metas[i], &metas[j]) < H_FACTOR*LOC_MAX ) ) 
		    {
		        hayruta = theta.search(metas[i], metas[j]);
		        dist = theta.get_dist();
                if(!hayruta)
                {
                    map.deactivate_safety_margin();
	    	        theta.clear_data();
    			    map.reset_data();
                    hayruta = theta.search(metas[i], metas[j]);
                    map.safety_margin((int)(ceil(desp/res)), desp);
    		        dist = theta.get_dist() * 1.5;
                }
		        theta.clear_data();
			    map.reset_data();
		    }
		    if(hayruta)
		    {
		        distancias[i][j] = dist;
		        distancias[j][i] = dist;
		        TSPdistancias[i][j] = dist;
		        TSPdistancias[j][i] = dist;
		    }
		    else
		    {
		        distancias[i][j] = NO_ALCANZABLE;
		        distancias[j][i] = NO_ALCANZABLE;
		        TSPdistancias[i][j] = NO_ALCANZABLE;
		        TSPdistancias[j][i] = NO_ALCANZABLE;
		        std::cerr << "Imposible alcanzar "; printPoint(metas[j]);
		        std::cerr << "desde "; printPoint(metas[i]);
		        if(i == 0)
		        {
		            std::cerr << "No se puede alcanzar un objetivo desde el estado inicial." << endl;
		            return -1;
		        }
		    }
    }
   // GRAFO DE DISTANCIAS. PROBLEMA TSP.
   // Calcular la distancia más corta por nodos alcanzados. 
   // En el problema de TSP no es realiza búsqueda exhaustiva, si no greedy, basada en la dist. más corta en cada punto 
   float distiniciales[nmetas-1];
   int i=0, nextx=0,nexty = 0;
   int cont =1;
   float mindist = FLT_MAX;  
   int visited [nmetas-1];
   memset (visited, 0, sizeof (visited));
   // 4 5 3 1 2  -- Si se ordenan las metas desde el estado inicial
   //4 5 2 1 3   -- Si te toma la distancia greedy desde cada punto
    while (cont < nmetas)    
    {    
        int change = 0; 
		mindist = FLT_MAX;
        //Si es la primera posición, poner NO ALCANZABLE para que el resto de posiciones no puedan volver al estado inicial
        for(int j = 0; j<nmetas; j++)
        {
           if (!i) 
				TSPdistancias[j][i] = TSPdistancias[j][i];
           else
           {
			  for (int k=0; k<nmetas-1; k++)
		             if (visited [k] != 0)
		             {
		                int pos = visited[k];
		             }
		             else
		                break;
           }

           if ((TSPdistancias[i][j] < mindist) && (TSPdistancias[i][j] > 0))
            {
                   mindist = TSPdistancias[i][j]; 
                   change=1; 
                   nextx = i;
                   nexty = j;
            }
       }
      
       distiniciales[cont-1] = distancias[0][nexty];
       visited[cont-1]=nexty;
       i=nexty;
       cont++;

    }

// Visualiza las distancias
    if(DEBUG){
     for(int i=0; i<nmetas; i++)
     {
        char valout[32];
        if(i == 0)
        {
            cout << "Dists ";
            for(int h=0; h<nmetas; h++)
            {
                sprintf(valout," [%02d]  ",h);
                cout << valout;
            }
            cout << endl;
        }
        sprintf(valout," [%02d] ",i);
        cout << valout;
        for(int j=0; j<nmetas; j++)
        {
            sprintf(valout,"%06.2f ",TSPdistancias[i][j]);
            cout << valout;
        }
        cout << endl;
    }
   }

 // EScribe las distancias en el fichero de PDDL
 for(int i = 0; i<nmetas; i++)   // i<nmetas
   for(int j = 1; j<nmetas; j++) // j<nmetas
    if(TSPdistancias[i][j] > 0)
     {
        char objs[T_PUNTO*4];
        char obj1[T_PUNTO];
        char obj2[T_PUNTO];
        getCelda(metas[i], obj1);
        getCelda(metas[j], obj2);
        sprintf(objs, "%s %s", obj1, obj2);
        if(problema.insertar_funcion((char*)FUNCION_DISTANCIA, objs, TSPdistancias[i][j]) != 0)
        {
            std::cerr << "Fallo al insertar la distancia en el problema PDDL." << endl;
            return -1;
        }
        if(DEBUG)
            cout << "(" << FUNCION_DISTANCIA << " " << objs <<  " " << TSPdistancias[i][j]  << ")  " << endl;
    }
    
    if(DEBUG)
    {
     cout << "Metas ordenadas: ";
     for(int i=0; i<nmetas-1; i++)
     {
         cout << distiniciales[i] << " " ;
     }
     cout << endl;
    }

    // Ejecutar TASK-PLANNER
    if(!lanzar_taskplanner(argv[2], argv[4]))
    {
        std::cerr << "Fallo en el Task-planner." << endl;
        return -1;
    }

    // Leer plan generado
    CPlan plan;
    if(plan.init((char*)OUT_TASK_PLAN) != 0)
    {
        std::cerr << "Fallo con el archivo generado por el task-planner." << endl;
        return -1;
    }
	std::cout << "SOLUCION:"<<std::endl<<std::endl;
    int numaccion = 1;
    char accion[256];
    char objetos[256];
	char c1[32];
	char c2[32];
    char** listaobjs;
	float distultimaloc=0;
    float duracion = 0;
    double coste_acumulado = 0;
    bool recargar = false;
    int xi,yi,xf,yf = 1;
    std::string Cadena;
    stringstream numaccion_str,duracionparcial_str;
    while(numaccion > 0)
    {
        numaccion = plan.sig_accion(accion, objetos, &duracion);
        // Reemplazar MOVE_TO por ruta
        if(!strcmp(accion, ACCION_MOVE))
        {
			listaobjs = new char*[plan.numero_objetos_proceso()];
            plan.lista_objetos(listaobjs);
			getCoords(listaobjs[1], &xi, &yi);
            getCoords(listaobjs[2], &xf, &yf);
            /*********************************Lineas Diego************************************/
            hayruta=theta.search(*map.get_point(xi,yi), *map.get_point(xf,yf));
            if (!hayruta)
            {
                                    map.deactivate_safety_margin();
                                    theta.clear_data();   
                                    map.reset_data();
                                    hayruta=theta.search(*map.get_point(xi,yi), *map.get_point(xf,yf));
                                    map.safety_margin((int)(ceil(desp/res)), desp);
            }
            /**********************************************************************************/
			if(hayruta) //Indica si hay ruta entre dos puntos ...
			{
			   float recorridototal = theta.get_dist();
			   vector<POINT*> ruta = theta.get_path();
			   int ax,ay;
			   float recorridoparcial = 0;
			   float duracionparcial = 0;
			   theta.clear_data();
			   map.reset_data();
			   for(int i=ruta.size()-2; i>-1; i--)
			   {
				    ax = ruta[i]->X;
				    ay = ruta[i]->Y;
					recorridoparcial = map.get_dist(xi, yi, ax, ay);
					if (recorridoparcial != -1) coste_acumulado += recorridoparcial;
					duracionparcial = (recorridoparcial * duracion) / recorridototal;
					getCelda(xi*RES, yi*RES, c1);
					getCelda(ax*RES, ay*RES, c2);
                    numaccion_str.str(""); //Limpia el numero de acción
                    duracionparcial_str.str(""); //Limpia la duración parcial
                    numaccion_str<<numaccion;// Conversión de un entero para poder introducirlo en un string
                    duracionparcial_str<<duracionparcial;// Conversión de un double para poder introducirlo en un string
				    Cadena = numaccion_str.str() + " " + ACCION_MOVE + " " + NOMBRE_ROBOT + " " + c1 + " " + c2 + " ["+ duracionparcial_str.str() + "]" +"\n"; //String completo preparado para pasarlo a minusculas
                    boost::to_lower(Cadena);// Conversión de la cadena de texto a minusculas;
                    cout<<Cadena;
					xi = ax;
				    yi = ay;
				}
				ruta.clear();
				delete[] (char*)listaobjs;
			}
			else
			{
			cout << "No se ha encontrado ruta."<<endl;
            printPoint(*map.get_point(xi,yi));
            cout<<"-->";
             printPoint(*map.get_point(xf,yf));
			}
        }
        else // Cualquier otra accion
	   if(strcmp(accion, FINAL_PLAN))
       {
           numaccion_str.str("");
           duracionparcial_str.str("");
           numaccion_str<<numaccion;
           duracionparcial_str<<duracion;
           Cadena =numaccion_str.str() + " " + accion +" "+ objetos + " [" + duracionparcial_str.str() + "]";
           boost::to_lower(Cadena);
           cout<<Cadena<<std::endl;
           	//cout << numaccion << " " << accion <<" "<< objetos << " [" << duracion << "]" << endl;
       }
    }

    // Calcular y mostrar tiempo de ejecucion y distancia total recorrida
	gettimeofday(&t_fin, NULL);
	double stime = (double)(t_fin.tv_sec + (double)t_fin.tv_usec/1000000) -
			       (double)(t_ini.tv_sec + (double)t_ini.tv_usec/1000000) ;
	char dat[64];
    sprintf(dat,"\n# Distancia total recorrida: %.3f", coste_acumulado);
    std::cerr << dat;
	sprintf(dat,"\n# Tiempo total empleado: %.3f milisegundos\n", stime * 1000.0);
	std::cerr << dat;

    // END
    metas.clear();
    balizas.clear();
    if(!DEBUG)
       remove(OUT_TASK_PLAN);
    return 0;
}

