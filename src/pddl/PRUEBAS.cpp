//#include <cstdlib>
//#include <cstdio>
//#include "Planner.h"
//
//
//int main(int argc, char** argv)
//{
//
//    printf("\nPruebas de planificacion, argc=%d\n",argc);
//
//    if(argc<2)
//    {
//        printf("Numero insuficiente de argumentos.\n");
//        return -1;
//    }

//    //PRUEBAS CPROBLEMA ========================================================
//    int ret=0;
//    CProblema* prueba= new CProblema();
//    prueba->init(argv[1]);
//    float valor;
//    char pred[TAM_LINEA];
//    char objs[TAM_LINEA];
//    strcpy(pred,(char*)"altura");
//    strcpy(objs,(char*)"C25");
//    valor=planificador.valor_funcion(pred,objs);
//    printf("\n===> %f <===\n",valor);
//    strcpy(pred,(char*)"orientacion");
//    if(planificador.objeto_pred((char*)pred,1))
//        printf("\n===> %s <===\n",pred);
//    strcpy(pred,(char*)"superior");
//    if(planificador.objeto_pred((char*)pred,2))
//        printf("\n===> %s <===\n",pred);
//    if(planificador.objeto_pred((char*)pred,8))
//        printf("\n===> %s <===\n",pred);
//    if(ret==0)
//    ret=prueba->insertar_pred("PRUEBA","NINGUNO",true);
//    if(ret==0)
//    ret=prueba->reemplazar_pred("PRUEBA","NINGUNO","UNO DOS");
//    if(ret==0)
//    ret=prueba->insertar_funcion("FUNCION1","OBJ",32.45);
//    if(ret==0)
//    ret=prueba->insertar_funcion("FUNCION2","OBJ2",3);
//    if(ret==0)
//    ret=prueba->reemplazar_funcion("FUNCION2","OBJ2",88);
//    if(ret==0)
//    ret=prueba->eliminar_predicado("puntocarga","C42");
//    if(ret==0){
//    //PRUEBAS CPROBLEMA

//    //PRUEBAS CDOMINIO =========================================================
//    int ret=0;
//    CDominio* prueba2 = new CDominio(argv[1]);
//    ret=prueba2->init(argv[1]);
//    printf("\nNumero de acciones leidas: %d \t Token accion: %d\n",prueba2->numero_acciones_dominio(),prueba2->token_nombre_accion());
//    staccion accionc;
//    for(int i=0; i<prueba2->numero_acciones_dominio(); i++)
//        if(prueba2->accion_completa(i,&accionc)==0)
//        {
//            printf("\nAccion: %s  Ruta PLEXIL: %s",accionc.nombre,accionc.rutaPlexil);
//            printf("\n Token accion: %d  Numero de objetos: %d  Token duracion: %d",accionc.tokens[0],accionc.tokens[1],accionc.tokens[2]);
//        }
//    printf("\n");
//    if(ret==0){
//    //PRUEBAS CDominio

//    //PRUEBAS CPlan ============================================================
//    int ret=0;
//    CPlan prueba3(argv[1]);
//    ret=prueba3.init(argv[2],argv[3]);
//    if(ret==0)
//    {
//        int numaccion=1;
//        float duracion=0;
//        char* accion = new char[31];
//        strcpy(accion,"");
//        char* objetos = new char[TAM_LINEA/2];
//        strcpy(objetos,"");
//        while(numaccion>0)
//        {
//            numaccion=prueba3.sig_accion(accion,objetos,&duracion);
//            printf("%s\n",prueba3.cadena_accion_proceso());
//            printf("Nº accion: %d => %s Duracion: %01.2f\n",numaccion,accion,duracion);
//            printf(" Nº objetos: %d Objetos: %s\n\n",prueba3.numero_objetos_proceso(),objetos);
//        }
//        delete[] accion;
//        delete[] objetos;
//    }
//    if(ret==0){
//    //PRUEBAS CPlan=============================================================

//    //PRUEBAS INTEGRACION ======================================================
//    Planner planificador(argv[1]);
//    int ret=planificador.planificar();
//    if(ret==0)
//    {
//        staccion accionc;
//        for(int i=0; i<planificador.numero_acciones_dominio(); i++)
//            if(planificador.accion_completa(i,&accionc)==0)
//            {
//                printf("\nAccion: %s  Ruta PLEXIL: %s",accionc.nombre,accionc.rutaPlexil);
//                printf("\n Token accion: %d  Numero de objetos: %d  Token duracion: %d",accionc.tokens[0],accionc.tokens[1],accionc.tokens[2]);
//            }
//        printf("\n");
//
//        int numaccion=1;
//        float duracion=0;
//        char* accion = new char[31];
//        char* objetos = new char[TAM_LINEA/2];
//        while(numaccion>0)
//        {
//            numaccion=planificador.sig_accion(accion,objetos,&duracion);
//            printf("%s\n",planificador.cadena_accion_proceso());
//            printf("Nº accion: %d => %s Duracion: %01.2f\n",numaccion,accion,duracion);
//            printf(" Nº objetos: %d Objetos: %s\n",planificador.numero_objetos_proceso(),objetos);
//            printf(" Conector accion: %s <> %s\n\n",accion,planificador.conector_accion());
//        }

//        printf("\nNumero de replanificaciones: %d\n",planificador.num_replanificaciones());
//        ret=planificador.planificar(REPLANIFICAR);
//        printf("\nNumero de replanificaciones: %d\n",planificador.num_replanificaciones());
//        planificador.planificar(REPLANIFICAR);
//        printf("\nNumero de replanificaciones: %d\n",planificador.num_replanificaciones());
//        char* obj=new char[31];
//        numaccion=1;
//        while(numaccion>0)
//        {
//                numaccion=planificador.sig_accion(accion,obj,&duracion);
//                printf("Nº accion: %d => %s Objetos: %s Duracion: %01.2f\n",numaccion,accion,obj,duracion);
//        }
//        planificador.planificar();
//        printf("\nNumero de replanificaciones: %d\n",planificador.num_replanificaciones());
//        Planner planificadordos(argv[1]);
//        numaccion=1;
//        planificadordos.planificar(true);
//        while(numaccion>0)
//        {
//                numaccion=planificadordos.sig_accion(accion,obj,&duracion);
//                printf("1|Nº accion: %d => %s Objetos: %s Duracion: %01.2f\n",numaccion,accion,obj,duracion);
//                numaccion=planificador.sig_accion(accion,obj,&duracion);
//                printf("2|Nº accion: %d => %s Objetos: %s Duracion: %01.2f\n",numaccion,accion,obj,duracion);
//        }
//        delete[] accion;
//        delete[] objetos;
//        return 0;
//    }
//    else //Fallo al planificar
//    {
//        switch(ret){
//        case -1:printf("\nFALLO AL ABRIR ARCHIVO\n"); break;
//        case -2:printf("\nFALLO AL BUSCAR PREDICADO/FUNCION\n"); break;
//        case -11:printf("\nFALLO AL ABRIR EL ARCHIVO DE CONFIGURACION (dominio)\n"); break;
//        case -12:printf("\nNO HAY ACCIONES EN EL ARCHIVO DE CONFIGURACION (dominio)\n"); break;
//        case -13:printf("\nNUMERO DE ACCIONES LEIDAS INCORRECTO\n"); break;
//        case -14:printf("\nPOSICION DEL VECTOR INCORRECTA O VECTOR VACIO\n"); break;
//        case -21:printf("\nPLAN NO ABIERTO\n"); break;
//        case -22:printf("\nPLAN SIN ACCIONES\n"); break;
//        case -23:printf("\nACCION DESCONOCIDA EN EL PLAN\n"); break;
//        case -31:printf("\nFALLO AL ABRIR EL ARCHIVO DE CONFIGURACION (planner)\n"); break;
//        case -32:printf("\nNO SE HAN LEIDO TODOS LOS DATOS NECESARIOS PARA EL PLANIFICADOR\n"); break;
//        case -33:printf("\nERROR AL CREAR EL PROCESO PARA EL PLANIFICADOR\n"); break;
//        default: printf("\nFALLO EN EL SISTEMA DE ARCHIVOS, DEL PLANIFICADOR O ERROR DESCONOCIDO\n");
//        }
//        return -1;
//    }
//}
