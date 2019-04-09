#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctype.h> 
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MENSAJE_HEURISTICA '0'
#define MENSAJE_CAMINO '1'
#define MENSAJE_SALIR '2'
#define MENSAJE_COSTE '3'

int leerPalabra(FILE *fichero, char *palabra) ;
int obtenerCoor(char* cadarchivo, char* objetivo, char** variables, int tam);
int quitarParentesis(char* palabra, int parentesis);
int comprobarCoor(char* palabra);
bool existeElemento (char* original, char** resultado, int cont);
int limpiarVector (char** variables, char** resultado, int tam);
int lecturaCompleta(FILE *fichero, char *palabra, bool frase);
int actualizarFichero(FILE* archivo, FILE* aux, double** matriz, char** caminos, int tam, char** ordenados, int tamaux);
int intercambioProcesos (char** variables, int tam, double** matriz, int fdr, int fdw);
int compararDoubles (const void * a, const void * b);
int ordenarPosiciones (char** caminos, char** ordenados, double** matriz, int tam, char** norepe);
char* buscarValor (double** matriz, char** caminos, int tam, double valor);
int sacarPalabra(char* frase, char* palabra, int cont);
int capturarAccion (char* frase, char* frasemod);
void generarPlan(char* dominio, char* problema);
int intercambioTask (char* algoritmo, char* zfile, char* cfile, char** variables, int tam, double** matriz, char** objetivos, int tamaux, char* cadena, char* dominio);
int actualizarRuta (FILE *archivo, FILE* ruta, char* frase, int coord1, int coord2, char* primera, bool vuelta, char* final);
int formatCoor(char* coor, char* frase);
bool esComent(char* frase);
int coorXiterador (char** vector, char* coor, int tam);

//PROGRAMA PRINCIPAL
int main(int argc, char *argv[])
{
  //Se comprueban los argumentos. Se introducen en orden: dominio.pddl problema.pddl .ALGORITMO zfile cfile
  if (argc!= 7 && argc!=6) {
    printf("Número de argumentos incorrecto.\nRecuerde: ./up2ta problema.pddl dominio.pddl .ALG z-file c-file\n");
    return -1;
  }
  else {
	char *cadena, *algoritmo, *dominio, *zfile, *cfile;
	if(argc == 6)
	{
	    cadena=(char*)malloc(strlen(argv[2])+1);
	    algoritmo=(char*)malloc(strlen(argv[2])+1);
		dominio=(char*)malloc(strlen(argv[1])+1);
		zfile=(char*)malloc(strlen(argv[4])+1);
		cfile=(char*)malloc(strlen(argv[5])+1);
		strcpy(cadena, argv[2]);
		strcpy(dominio, argv[1]);
		strcpy(algoritmo,argv[3]);
		strcpy(zfile, argv[4]);
		strcpy(cfile, argv[5]);
	}
	else
	{
	    cadena=(char*)malloc(strlen(argv[4])+1);
	    algoritmo=(char*)malloc(strlen(argv[5])+1);
		dominio=(char*)malloc(strlen(argv[2])+1);
		zfile=(char*)malloc(sizeof(char));
		cfile=(char*)malloc(sizeof(char));
		strcpy(cadena, argv[4]);
		strcpy(dominio, argv[2]);
		strcpy(algoritmo,argv[5]);
		zfile[0]='\0';
		cfile[0]='\0';
	}
    char end[5];
    strcpy(end,".pddl");
    size_t lenstr = strlen(cadena);
    size_t lenstrdom = strlen(dominio);
    size_t lensuffix = strlen(end);

    //Se comprueba que los archivos introducidos tienen la extensión PDDL
    if ((lensuffix > lenstr) || (lensuffix > lenstrdom)){
      printf("Introduzca como argumentos archivos PDDL\nRecuerde: ./up2ta dominio.pddl problema.pddl  .ALG z-file c-file\n");
      return -1;
    }
    else if (strncmp(cadena + lenstr - lensuffix, end, lensuffix) != 0) {
      printf("Introduzca como argumentos archivos PDDL\nRecuerde: ./up2ta dominio.pddl problema.pddl  .ALG z-file c-file\n");
      return -1;

    } else if (strncmp(dominio + lenstrdom - lensuffix, end, lensuffix) != 0) {
      printf("Introduzca como argumentos archivos PDDL\nRecuerde: ./up2ta dominio.pddl problema.pddl  .ALG z-file c-file\n");
      return -1;
	}
    //Se ha introducido archivos con la extensión PDDL. Seguimos.
    else {
			//Inicializacion del vector donde se guardan las posiciones leidas
			char **variables = (char **)malloc(50*sizeof(char*)); 
			int tam = 0;
			int cont = 0;
			int i;
			//Leer posición inicial
			if((tam=obtenerCoor(cadena, (char*)"(position", variables, tam))==-1) {
				printf("No se ha detectado posición inical\n");
				free(variables);
				return -1;
			}
			//Leer objetivos
			if((tam=obtenerCoor(cadena, (char*)"(:goal", variables, tam))==-1) {
				printf("No se han detectado posiciones a las que ir\n");
				free(variables);
				return -1;
			}
			int tamaux=tam-1;
			char **objetivos = (char **)malloc((tam)*sizeof(char*));
			for(i=0;i<tam;i++) {
				objetivos[i]=(char*)malloc(10*sizeof(char));
				strcpy(objetivos[i],variables[i]);
			}
			//Leer Estaciones de Recarga (si las hay)
			if((tam=obtenerCoor(cadena, (char*)"(dock", variables, tam))==-1) {
				printf("No se han detectado posiciones de recarga\n");
			}
			char **caminos = (char **)malloc(tam*sizeof(char*));
			for(i=0;i<tam;i++) {
				caminos[i]=(char*)malloc(10*sizeof(char));
			}
			//Eliminar objetivos duplicados
			int tamMatrix= limpiarVector (variables, caminos, tam);
			//Calcular costes
			double** matriz= (double**)malloc(tamMatrix*sizeof(double*));
			for(i=0;i<tamMatrix;i++) {
				matriz[i]=(double*)malloc(tamMatrix*sizeof(double));
			}
			//Mezclar el plan del task-planner con el path-planner
			if(intercambioTask(algoritmo,zfile,cfile,caminos, tamMatrix, matriz, objetivos, tamaux, cadena, dominio)==-1) {
				printf("Hubo un error al integrar task-planer con path-planner\n");
				remove("/tmp/tuberia-astar-ff");
				remove("/tmp/tuberia-ff-astar");
			}
			fcloseall(); 
			remove("/home/gsi/Escritorio/mobar/bin/planner/logPathPlanner");
			remove("/home/gsi/Escritorio/mobar/bin/planner/logSGplan");
			//Liberar memoria
			free(caminos);
			free(variables);
			free(matriz);
			free(cadena);
			free(algoritmo);
			free(dominio);
			free(zfile);
			free(cfile);
			return 0;
		}
	}
}

/* 
	Funcion que integra el plan generado por el taskplanner con las rutas calculadas
	con el pathplanner. 
	Los argumentos son: El algoritmo deseado (ver a continuación los disponibles), el
	archivo z y el archivo c.
	Algoritmos (siempre con punto delante)
	.a -> A*, .d -> Dijkstra, .t -> Basic Theta*
	.s -> S-Theta*, .b -> A*PS Greedy, .v -> Basic Theta* Greedy
	.A -> A*, .D -> Dijkstra, .L -> Lineal G-Theta*
*/
int intercambioTask (char* algoritmo, char* zfile, char* cfile, char** variables, int tam, double** matriz, char** objetivos, int tamaux, char* cadena, char* dominio) {
	//Creación de la tubería
	int fifo = mkfifo("/tmp/tuberia-ff-astar", 0777);
	int fifo2= mkfifo("/tmp/tuberia-astar-ff", 0777);
	int fdw, fdr;
	if (fifo<0 || fifo2<0) {
		printf("Error al crear la tubería\n");
		return -1;			
	}
	else 
	{
		pid_t pid;
		pid = fork();
		if(pid == -1) return -1;
		if(pid==0) { //HIJO
			char aux[200];
			aux[0]='\0';
			strcat(aux,"java -jar /home/gsi/Escritorio/mobar/bin/planner/PathPlanner.jar ");
			strcat(aux,algoritmo);
			strcat(aux," ");
			strcat(aux,zfile);
			strcat(aux," ");
			strcat(aux,cfile);
			strcat(aux," > /home/gsi/Escritorio/mobar/bin/planner/logPathPlanner");
			system(aux);
		}
		else 
		{	//Padre
			fdr=open("/tmp/tuberia-astar-ff",O_RDONLY);
			fdw=open("/tmp/tuberia-ff-astar",O_WRONLY);
			if(fdw<0 || fdr<0) {
				printf("Error apertura tuberia\n");
				remove("/tmp/tuberia-ff-astar");
				remove("/tmp/tuberia-astar-ff");
				return -1;
			}
			//Calcular costes entre pares de puntos mediante pathplanner
			intercambioProcesos(variables, tam, matriz, fdr, fdw);

			int i;
			//Ordenar objetivos (desde inicio) por menor coste
			char **ordenados = (char **)malloc((tamaux)*sizeof(char*));
			for(i=0;i<tamaux;i++) {
				ordenados[i]=(char*)malloc(10*sizeof(char));
			}
			ordenarPosiciones(objetivos, ordenados,matriz,tamaux+1,variables);
			
			free(objetivos);
			//Actualizar fichero PDDL con los costes calculados
			FILE *aux;
			FILE *archivo;
			archivo=fopen(cadena, "r");
			if(archivo==NULL){
				printf("Error al abrir el fichero %s\n",cadena);
				int opcion= MENSAJE_SALIR;
				if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion de salida\n");}
			
				close(fdr);
				close(fdw);
				remove("/tmp/tuberia-ff-astar");
				remove("/tmp/tuberia-astar-ff");
				return -1;
			}
			aux=fopen("aux.pddl", "w");
			if(aux==NULL){
				printf("Error al actualizar el fichero PDDL\n");
				int opcion= MENSAJE_SALIR;
				if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion de salida\n");}
			
				close(fdr);
				close(fdw);
				remove("/tmp/tuberia-ff-astar");
				remove("/tmp/tuberia-astar-ff");
				return -1;
			}
			actualizarFichero(archivo,aux, matriz, variables, tam, ordenados, tamaux);				
			
			fclose(archivo);
			fclose(aux);
			remove(cadena);
			rename("aux.pddl",cadena);
			printf("\nEl archivo PDDL ha sido actualizado\n");
			free(ordenados);
			//Llamar al task-planner y generar el plan
			generarPlan(dominio,cadena);
			FILE *plan;
			FILE *solucion;
			//El plan generado por el SGplan6 estará en el archivo "plan"
			plan=fopen("/home/gsi/Escritorio/mobar/bin/planner/logSGplan", "r");
			if(plan==NULL){
				printf("Error al abrir el plan generado por SGplan6\n");
				int opcion= MENSAJE_SALIR;
				if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion de salida\n");}
			
				close(fdr);
				close(fdw);
				remove("/tmp/tuberia-ff-astar");
				remove("/tmp/tuberia-astar-ff");
				return -1;
			}
			//La solución se guardará en el archivo "/home/gsi/Escritorio/mobar/bin/planner/solucion"
			solucion=fopen("/home/gsi/Escritorio/mobar/bin/planner/solucion","w");
			if(solucion==NULL){
				printf("Error al crear/abrir el fichero solución\n");
				int opcion= MENSAJE_SALIR;
				if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion de salida\n");}
			
				close(fdr);
				close(fdw);
				remove("/tmp/tuberia-ff-astar");
				remove("/tmp/tuberia-astar-ff");
				return -1;
			}
			fputs("SOLUCION\n\n",solucion);
			int opcion;
			char coordenadai[10];
			char coordenadaj[10];
			char coordenadarcv[10];
			int j;
			for(i=0;i<10;i++) {
				coordenadai[i]='\0';
				coordenadaj[i]='\0';
				coordenadarcv[i]='\0';
			}
			char frase[100];
			char mod[100];
			char palabra[100];
			for(i=0;i<100;i++){
				frase[i]='\0';
				mod[i]='\0';
				palabra[i]='\0';
			}
			int cont=0;
			int move;
			int longitud;
			bool vuelta=true;
			//Va leyendo las líneas del plan original. Si tiene MOVETO, se desglosa
			//con la ruta que devuelva el pathplanner.
			while(lecturaCompleta(plan, frase,false)!=0) {
				move=0;
				longitud= strlen(frase);
				if(longitud>2) {
					capturarAccion(frase, mod);
					//Se comprueba si es MOVETO
					while(cont!=strlen(mod)) {
						cont= sacarPalabra(mod, palabra, cont);
						if(strcmp(palabra,"MOVETO")==0){
							move=1;
							do{
								for(i=0;i<100;i++){
									palabra[i]='\0';
								}
								cont=sacarPalabra(mod,palabra,cont);
							}	while (comprobarCoor(palabra)!=1);
							strcpy(coordenadai,palabra);
							j=cont-strlen(palabra);
							for(i=0;i<100;i++){
								palabra[i]='\0';
							}
							cont=sacarPalabra(mod,palabra,cont);
							strcpy(coordenadaj,palabra);

							//COMUNICACION CON PATHPLANNER	
							opcion= MENSAJE_CAMINO;
							//Se manda la opción para calcular el coste entre dos puntos
							if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion\n");}
							//Se manda coordenada inicial
							if (write(fdw,coordenadai,sizeof(coordenadai))<0) {printf("Error al escribir coordenada inicial\n");}
							//Se recibe confirmacion de la coordenada inicial
							if (read(fdr,coordenadarcv,sizeof(coordenadarcv))<0) {printf("Error al confirmar coordenada inicial\n");}
							//Se envía la coordenada final.
							if (write(fdw,coordenadaj,sizeof(coordenadaj))<0) {printf("Error al escribir coordenada final\n");}
							//Se recibe confirmacion de la coordenada final
							if (read(fdr,coordenadarcv,sizeof(coordenadarcv))<0) {printf("Error al confirmar coordenada final\n");}
							FILE *ruta;
							ruta=fopen("/home/gsi/Escritorio/mobar/bin/planner/logPathPlanner","r+");
							if(ruta==NULL) {printf("Error al leer el plan del pathplanner\n"); return -1;}
							actualizarRuta (solucion, ruta,mod, j, cont, coordenadai,vuelta, coordenadaj);
							fclose(ruta);
							vuelta=false;
						}
						for (i=0;i<100;i++) {
							palabra[i]='\0';
						}
					}
					if(move==0) {fputs(frase,solucion);}
				}
				cont=0;
				for(i=0;i<100;i++){
					frase[i]='\0';
					mod[i]='\0';
				}
			}

			fclose(plan);
			fclose(solucion);
			opcion= MENSAJE_SALIR;
			if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion de salida\n");}
			close(fdr);
			close(fdw);
			printf("Generado el fichero solucion.\n");
			waitpid(pid,NULL,0);
		}
	}
	remove("/tmp/tuberia-ff-astar");
	remove("/tmp/tuberia-astar-ff");
	return 0;
}

/*
	Función que una vez generada la ruta por el pathplanner, se le pasa
	una línea MOVETO y escribe en el fichero solución el desglose de la ruta.
	Los argumentos son: el archivo solucion, la linea MOVETO, un puntero
	al lugar de la frase donde sustituir la coordenada 1 y un puntero al
	lugar de la frase donde se acaba la coordenada 2.
*/
int actualizarRuta (FILE *archivo, FILE* ruta, char* frase, int coord1, int coord2, char* primera, bool vuelta, char* final) {

	int cont=0;
	char palaux[100];
	char fin[100];
	char num[10];
	char auxfin[10];
	char resto[100];
	int i,j, doble;	
	char puntero[10];
	char comp[10];
	for(i=0;i<100;i++) {
		fin[i]='\0';
		palaux[i]='\0';
		resto[i]='\0';
	}
	for(i=0;i<10;i++) {
		num[i]='\0';
		puntero[i]='\0';
		comp[i]='\0';
		auxfin[i]='\0';
	}
	strcpy(fin,"000: ");
	for(i=0;i<coord1;i++) {
		fin[i+5]=frase[i];
	}
	for(i=coord2;i<strlen(frase);i++) {
		resto[cont]=frase[i];
		cont++;
	}
	cont=0;
	doble=1;
	int punt=0;
	int k;
	formatCoor(primera,puntero);

	while(punt==0) {
		for(i=0;i<100;i++) {
			palaux[i]='\0';
		}
		lecturaCompleta(ruta, palaux,false);
		for(k=0;k<strlen(puntero);k++){
			comp[k]=palaux[k]; 
		}
		if (strcmp(comp,puntero)==0) {
			punt=1;		
		}
		for(i=0;i<10;i++) {
			comp[i]='\0';
		}
	}
	if(vuelta==false) {
		lecturaCompleta(ruta, palaux,false);
	}
	do {
		if(punt!=0){
			if((isdigit(palaux[0])!=0) && (doble==1)) {
				cont=sacarPalabra(palaux, num,cont);
				strcat(fin,num);
				strcat(fin,"_");
				for(i=0;i<10;i++) {
					num[i]='\0';
				}
				cont=sacarPalabra(palaux, num,cont);
				strcat(fin,num);
				strcat(fin," ");
				for(i=0;i<10;i++) {
					num[i]='\0';
				}
				doble++;
				cont=0;
			}
			else if((isdigit(palaux[0])!=0) && (doble==2)) {
				cont=sacarPalabra(palaux, num,cont);
				for(i=0;i<10;i++) {
					auxfin[i]='\0';
				}	
				strcpy(auxfin,"C");
				strcat(auxfin,num);
				strcat(auxfin,"_");	
				strcat(fin,"C");
				strcat(fin,num);
				strcat(fin,"_");
				for(i=0;i<10;i++) {
					num[i]='\0';
				}
				cont=sacarPalabra(palaux, num,cont);
				strcat(auxfin,num);
				strcat(fin,num);
				strcat(fin," ");
				for(int r=0; r<strlen(resto); r++)
				    if(resto[r] == '\n')
				        resto[r]='\0';
				strcat(fin,resto);
				strcat(fin," [1]\n");
				for(i=0;i<10;i++) {
					num[i]='\0';
				}
				doble=1;
				cont=0;
				fputs(fin,archivo);
				for(i=0;i<100;i++) {
					fin[i]='\0';
				}
				strcpy(fin,"000: ");
				for(i=0;i<coord1;i++) {
					fin[i+5]=frase[i];
				}
			}
		} 
		for(i=0;i<100;i++) {
			palaux[i]='\0';
		}
	}while(lecturaCompleta(ruta, palaux,false)!=0);

	if(doble==2){
		for(i=0;i<100;i++) {
			fin[i]='\0';
		}
		strcpy(fin,"000: ");
		for(i=0;i<(coord1-1);i++) {
			fin[i+5]=frase[i];
		}
		strcat(fin,auxfin);
		strcat(fin," ");
		strcat(fin,final);
		strcat(fin," ");
		for(int r=0; r<strlen(resto); r++)
		    if(resto[r] == '\n')
		        resto[r]='\0';
		strcat(fin,resto);
		strcat(fin," [1]\n");
		fputs(fin,archivo);
	}
	return 0;
}

/*
	Función que hace una llamada al SGPlan6
	Los argumentos son: el dominio y el problema (debidamente actualizado)
*/
void generarPlan(char* dominio, char* problema) {
	int i;
	char aux[200];
	for(i=0;i<200;i++){
		aux[i]='\0';
	}
	strcat(aux,"/home/gsi/Escritorio/mobar/bin/planner/sgplan -o ");
	strcat(aux,dominio);
	strcat(aux, " -f ");
	strcat(aux,problema);
	strcat(aux," -out /home/gsi/Escritorio/mobar/bin/planner/logSGplan > /dev/null");
	system(aux);
}

int ordenarPosiciones (char** caminos, char** ordenados, double** matriz, int tam,  char** norepe) {
	int i;
	double valores[tam-1];
	int cont=0;
	for (i=1;i<tam;i++) {
		valores[cont]=matriz[0][i];
		cont++;
	}
	qsort(valores, tam-1, sizeof(double), compararDoubles);
	
	for (i=0;i<(tam-1);i++) {
		strcpy(ordenados[i],buscarValor(matriz,norepe,tam,valores[i]));
	}
    return 0;
}

char* buscarValor (double** matriz, char** caminos, int tam, double valor) {
	
	int i,j;
	if(valor<0.0000001) {
		return caminos[0];
	}
	for(i=0;i<tam;i++) {
		if(matriz[0][i]==valor){
			return caminos[i];
		}
	}
	return NULL;
} 

int compararDoubles (const void * a, const void * b) {
  if ( *(double*)a <  *(double*)b ) return -1;
  if ( *(double*)a == *(double*)b ) return 0;
  if ( *(double*)a >  *(double*)b ) return 1;
}

/*
	IntercambioProcesos calcula el coste entre pares de puntos mediante el pathplanner y se
	guardan en la matriz.
	Se supone ya abierta la comunicación con éste, así que se comunican los datos mediante
	las tuberías: fdr para leer y fdw para escribir
*/
int intercambioProcesos (char** variables, int tam, double** matriz, int fdr, int fdw) {

	char coordenadai[10];
	char coordenadaj[10];
	char coordenadarcv[10];
	int i, j;
	for(i=0;i<10;i++) {
		coordenadai[i]='\0';
		coordenadaj[i]='\0';
		coordenadarcv[i]='\0';
	}
	int opcion;
	opcion= MENSAJE_COSTE;
	char coste[14];

	for (i=0;i<tam;i++) {
		strcpy(coordenadai,variables[i]);
		for(j=i;j<tam;j++) {
			strcpy(coordenadaj,variables[j]);

			if (strcmp(coordenadai,coordenadaj)!=0) 
			{ //Las coordenadas NO son la misma.
				//Se manda la opción para calcular el coste entre dos puntos
				if (write(fdw,&opcion,sizeof(opcion))<0) {printf("Error al escribir opcion\n");}
				//Se manda coordenada inicial
				if (write(fdw,coordenadai,sizeof(coordenadai))<0) {printf("Error al escribir coordenada inicial\n");}
				//Se recibe confirmacion de la coordenada inicial
				if (read(fdr,coordenadarcv,sizeof(coordenadarcv))<0) {printf("Error al confirmar coordenada inicial\n");}
				//Se envía la coordenada final.
				if (write(fdw,coordenadaj,sizeof(coordenadaj))<0) {printf("Error al escribir coordenada final\n");}
				//Se recibe confirmacion de la coordenada final
				if (read(fdr,coordenadarcv,sizeof(coordenadarcv))<0) {printf("Error al confirmar coordenada final\n");}
				//Se recibe el coste.
				if (read(fdr,&coste,sizeof(coste))<0) {printf("Error al recibir coste\n");}
				
				double sol;
				sol=atof(coste);
				matriz[i][j]=sol;
				matriz[j][i]=sol;
			}
			else {	//Las coordenadas son la misma -> coste 0.
				matriz[i][j]=0;
			}
		}
	}
	return 0;
}

int limpiarVector (char** variables, char** resultado, int tam) {
	int i;
	int cont=0;

	for(i=0;i<tam;i++) {
		if (!existeElemento(variables[i], resultado, cont)) {
			resultado[cont]=variables[i];
			cont++;
		}
	}
	return cont;
}

bool existeElemento (char* original, char** resultado, int cont) {
	int i;
	for(i=0;i<cont;i++) {
		if(strcmp(original,resultado[i])==0) {
			return true;
		}
	}
	return false;
}

int obtenerCoor(char* cadarchivo, char* objetivo, char** variables, int tam) {
	char palabra[20];
	int i;
	for(i=0;i<20;i++){
		palabra[i]='\0';
	}
	int encontrado=1;
	int parentesis=1;
	
	FILE *archivo;
	archivo=fopen(cadarchivo, "r");
	//Si no se pudo abrir el archivo...
	if(archivo==NULL) {
		printf("Error al intentar abrir el archivo %s\n",cadarchivo);
		return -1;
	}
	
	while(leerPalabra(archivo,palabra)!=0) {
		encontrado= strcmp(objetivo,palabra);
		if (encontrado==0) {
			while(leerPalabra(archivo,palabra)!=0) {

				if (parentesis==0) {
					if(strcmp("(dock",objetivo)==0) {
						if(strcmp("(dock",palabra)==0) {
							
							parentesis++;
						} else {
							fclose(archivo);
							return tam;
						}
					} else { 
						fclose(archivo);
						return tam;
					}
				} else if(palabra[0] == '(') {
					parentesis++;
				}
				if(palabra[0] == ')') {
					parentesis--;
				}
				parentesis=quitarParentesis(palabra,parentesis);
				if (palabra[0] == 'C') {
					if(comprobarCoor(palabra)==1) {
						variables[tam]=(char*)malloc(20*sizeof(char));
						strcpy(variables[tam],palabra);
						tam++;
					}	
				}
				for(i=0;i<20;i++){
					palabra[i]='\0';
				}
			}
		}
		for(i=0;i<20;i++){
			palabra[i]='\0';
		}
	}
	if(encontrado==0) {
		fclose(archivo);
		return tam;
	}
	if(strcmp("(dock",objetivo)==0) {
			printf("Aviso: No se detectaron estaciones de recarga\n");
			fclose(archivo);
			return tam;
	} 
	else {
		printf("No se encuentra %s\n",objetivo);
		fclose(archivo);
		return -1;
	}	
}

int quitarParentesis(char* palabra, int parentesis) {
	
	int i;
	for(i=0;i<20;i++) {
		if (palabra[i]==')') {
			parentesis--;
			palabra[i]='\0';
		}
	}	
	return parentesis;
}

int comprobarCoor(char* palabra) {
	int pos = 1;
	while (isdigit(palabra[pos])!=0) {
		pos++;
	}
	if (palabra[pos] != '_') {
		return 0;
	}
	pos += 1;
	while (isdigit(palabra[pos])!=0) {
		pos++;
	}
	return 1;
}

int lecturaCompleta(FILE *fichero, char *palabra, bool frase) {
	int caracterAux;
	int cont = 0;

	if(frase==true) {
		while ((caracterAux = fgetc(fichero)) != EOF) {
			if (caracterAux == '\n') {
				palabra[cont] = caracterAux;
				return -1;
			}
			palabra[cont] = caracterAux;
			cont++;
		}
		return 0;
	} else {
		bool comentario=false;
		while ((caracterAux = fgetc(fichero)) != EOF) {
			if (caracterAux== ';') {
				comentario=true;
			}
			if (comentario && caracterAux=='\n') {
				return 1;
			} 
			if (!comentario) {
				if (caracterAux == '\n') {
					palabra[cont] = caracterAux;
					return 1;
				}
				palabra[cont] = caracterAux;
				cont++;
			}
		}
		return 0;
	}
} 

int actualizarFichero(FILE* archivo, FILE* aux, double** matriz, char** caminos, int tam, char** ordenados, int tamaux) {
	char palabra[200];
	char paux[20];
	int i, j, m;
	int cont=0;
	int k=0;
	for(i=0;i<200;i++){
		palabra[i]='\0';
	}
	for(i=0;i<20;i++){
		paux[i]='\0';
	}
	char **goals = (char **)malloc(tamaux*sizeof(char*));
	for(i=0;i<tamaux;i++) {
		goals[i]=(char*)malloc(150*sizeof(char));
		for(m=0;m<150;m++) {
			goals[i][m]='\0';
		}
	}	

	while(lecturaCompleta(archivo,palabra,true)!=0) 
	{
		//Actualiza los "(:objects" -> Deja los que está, y añade los nuevos goals o dockstation que pueda haber
		if ((strcmp("(:objects\r\n",palabra)==0) || (strcmp("(:objects\n",palabra)==0)) {
			fputs(palabra, aux);
			lecturaCompleta(archivo,palabra,true);
			for(i=0;i<200;i++){
				palabra[i]='\0';
			}
			strcat(palabra," ");
			for(i=0;i<tam;i++) {
				//recorrer el vector y ponerlo en cadena.
				strcat(palabra,caminos[i]);
				strcat(palabra," "); 
			}
			strcat(palabra,"- loc\n");
			fputs(palabra, aux);
		}
		//Actualiza el fichero .pddl con los costes calculados	
		else if ((strcmp("(:init\r\n",palabra)==0) || (strcmp("(:init\n",palabra)==0)) {
			
			fputs(palabra,aux);
			fputs(";===============================================================================\n",aux);
			fputs(";LOCATION AND COST\n",aux);
			for (i=0;i<tam;i++) {
				for (j=0;j<tam;j++) {
					if (j!=i) {
						fprintf(aux,"(= (distance_to_move %s %s) %f)\n", caminos[i], caminos[j], matriz[i][j]);
					}
				}
			}
			//Si ya había costes anteriormente escritos, no los escribe (actualización)
		} else if(strcmp(";LOCATION AND COST\n",palabra)==0 || (strcmp(";LOCATION AND COST\r\n",palabra)==0) || (strcmp(";LOCATION AND DISTANCES\r\n",palabra)==0) || (strcmp(";LOCATION AND DISTANCES\n",palabra)==0)) {
			do {
				for(i=0;i<200;i++){
					palabra[i]='\0';
				}
				lecturaCompleta(archivo,palabra,true);
			} while (strcmp(";===============================================================================\n",palabra)==1);
			
		}
		else if ((strcmp("(:goal (and\r\n",palabra)==0) || (strcmp("(:goal (and\n",palabra)==0)) {
			int iterador;
			fputs(palabra, aux);
			do {
				for(i=0;i<200;i++){
					palabra[i]='\0';
				}
				lecturaCompleta(archivo,palabra,true);
				cont=0;
				if(esComent(palabra)==false) {
					while(cont<strlen(palabra)) {
						cont=sacarPalabra(palabra,paux,cont);
						quitarParentesis(paux,1);
						if(comprobarCoor(paux)==1) {
							iterador=coorXiterador(ordenados,paux,tamaux);
							if(goals[iterador][0]!='\0') {
								strcpy(goals[iterador+1],palabra);
							}
							else {
								strcpy(goals[iterador],palabra);
							}
						}
						for(i=0;i<20;i++){
							paux[i]='\0';
						}
					}			
				}
			} while (strcmp(")\n",palabra)==1);
			
			for(i=0;i<tamaux;i++){
				fputs(goals[i], aux);
			}
			fputs(palabra, aux);
		} else {
			fputs(palabra,aux);
		}
		for(i=0;i<200;i++){
			palabra[i]='\0';
		}
	}
	free(goals);
	return 0;
}

bool esComent(char* frase) {
	int i;
	for(i=0;i<strlen(frase);i++) {
		if (frase[i]==';') {
			return true;		
		}
	}
	return false;
}

int coorXiterador (char** vector, char* coor, int tam) {
	int i;
	for(i=0;i<tam;i++) {
		if(strcmp(vector[i],coor)==0) {
			return i;
		}
	}
	return tam;
}

int leerPalabra(FILE *fichero, char *palabra) {
	int caracterAux;
	int cont = 0;
	bool comentario=false;

	while ((caracterAux = fgetc(fichero)) != EOF) {

		if (caracterAux== ';') {
			comentario=true;
		}
		if (comentario && caracterAux=='\n') {
			return 1;
		}
		if (!comentario) {
			if ((caracterAux == ' ') || (caracterAux == '\n')) {
				return 1;
			}
			palabra[cont] = caracterAux;
			cont++;
		}
	}
	return 0;
} 

int sacarPalabra(char* frase, char* palabra, int cont) {
	int i=0;
	while(frase[cont]!=' ') {
		if(frase[cont]=='\n') {
			cont++;
			return cont;
		}
		else {
			palabra[i]=frase[cont];
			cont++;
			i++;
		}
	}
	cont++;
	return cont;
}

int capturarAccion (char* frase, char* frasemod) {
	int longitud=strlen(frase);
	int parentesis=0;
	int i=0;
	int cont=0;

	while(i<longitud) {
		if(frase[i]=='(') {
			parentesis++;
			i++;
		}
		if(frase[i]==')') {
			parentesis--;
			frasemod[cont]='\n';
			return 1;
		}
		if(parentesis>0) {
			frasemod[cont]=frase[i];
			cont++;
		}
		i++;
	}
	return 0;
}

int formatCoor(char* coor, char* frase) {
	int i,cont;
	cont=0;
	for(i=0;i<strlen(coor);i++) {
		if(isdigit(coor[i])!=0){
			frase[cont]=coor[i];
			cont++;		
		} else if (coor[i]=='_') {
			frase[cont]=' ';
			cont++;
		}
	}
	return 0;
}

