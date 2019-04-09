#include "PDDLAdapter.h"

// === REGISTRAR EL ADAPTADOR ===
extern "C" {
	void initPDDLAdapter() {
		REGISTER_ADAPTER(PLEXIL::PDDLAdapter,"PDDLAdapter");
	}
}
// ==============================

using PLEXIL::LabelStr;
using PLEXIL::State;
using std::cout;
using std::cerr;
using std::endl;
using std::list;
using std::map;
using std::string;
using std::vector;
using std::copy;

namespace PLEXIL
{

    PDDLAdapter::PDDLAdapter(AdapterExecInterface& execInterface, const pugi::xml_node& xml)
    : InterfaceAdapter(execInterface, xml)
    {
        assertTrue(!xml.empty(), "XML config file not found in PDDLAdapter::PDDLAdapter constructor");
        debugMsg("PDDLAdapter", " Using "<<xml.attribute("AdapterType").value());
    }

    PDDLAdapter::~PDDLAdapter()
    {
        debugMsg("PDDLAdapter", " Destructor called");
    }

    bool PDDLAdapter::initialize()
    {
        debugMsg("PDDLAdapter", " Initialize called...");
        m_execInterface.defaultRegisterAdapter(getId());
        ogatePlanner = OgateServerClient("PDDLAdapter");
        planificador = std::vector<Planner*>();
        ultimaacc = std::vector<stpredicado*>();
        tcreplan = false;
        running = 0;
        debugMsg("PDDLAdapter", " done");
        return true;
    }
    bool PDDLAdapter::start()
    {
        debugMsg("PDDLAdapter", " Started");
        return true;
    }
    bool PDDLAdapter::stop()
    {
        debugMsg("PDDLAdapter", " Stop");
        running = 0;
        if(ogatePlanner.hasControlInterface())
            sendStatusToOgate(&ogatePlanner, tick, COMPONENT_FINISHED);
        void* retval;
        pthread_join(ogClientThread, &retval);
        return true;
    }
    bool PDDLAdapter::reset()
    {
        debugMsg("PDDLAdapter", " Reset");
        return true;
    }
    bool PDDLAdapter::shutdown()
    {
        debugMsg("PDDLAdapter", " Shutdown");
        return true;
    }
    
    void PDDLAdapter::getXYposition(const LabelStr square, int *px, int *py,bool *success)
    {
    // Position x,y
    char aux[16];
    char* temptok;
    strcpy(aux, square.c_str());
    aux[0] = ' '; // Delete first 'C'
    temptok=strtok(aux, (char*)POS_DELIMITER); // First token, Y position
    if(temptok == NULL)
        *success = false; // Malformed square
    else
	*py = atoi(temptok);
    // Next token
    temptok=strtok(NULL, (char*)POS_DELIMITER); // Second token, X position
    if(temptok == NULL)
        *success = false; // Malformed square
    else
	*px = atoi(temptok);
    *success = true;
    }
    
    void* PDDLAdapter::ogateClientThread(void* this_as_void_ptr)
    {
        debugMsg("PDDLAdapter","Started TC PDDL thread.");
        PDDLAdapter* pddlInstance = reinterpret_cast<PDDLAdapter*>(this_as_void_ptr);
        int type, rec, status;
        bool finished = false;
        std::string message;
        while(!finished)
        {
            if(pddlInstance->ogatePlanner.isConnected())
            {
              status = pddlInstance->running>0?COMPONENT_RUNNING:COMPONENT_FINISHED;
              if(status == COMPONENT_FINISHED) // IF ALL PLANNERS FINISHED > SEND OGATE FINISH!
                    sendStatusToOgate(&pddlInstance->ogatePlanner, tick, status);

              rec = pddlInstance->ogatePlanner.receive(&type, &message);
              if(rec > 0 && type == CONTROL)
              {
                if(!message.compare(0, CONTROL_REQUEST.length(), CONTROL_REQUEST))
                {
                    if(!message.compare(CONTROL_IF_STATUS))
                    {
                        sendStatusToOgate(&pddlInstance->ogatePlanner, tick, status);
                    }
                    if(!message.compare(CONTROL_IF_FINISH)) // Stop
                    {
                        if(status == COMPONENT_RUNNING)
                            status = COMPONENT_FAILURE;
                        sendStatusToOgate(&pddlInstance->ogatePlanner, tick, status);
                        finished = true;
                    }
                } // CONTROL REQUEST
                else // TELECOMMAND
                {
                    if(pddlInstance->planificador.size()>0)
                    {
                        int objstart = message.find_first_of(' '); 
                        std::string pred(message.substr(0, objstart));
                        std::string objs(message.substr(objstart, message.length()));
                        if(pddlInstance->planificador[0]->insertar_pred((char*)pred.c_str(), (char*)objs.c_str(), true) == 0)
                        {   
                            debugMsg("PDDLAdapter","GOAL ADDED: ("<<pred<<" "<<objs<<")");
                            pddlInstance->tcreplan = true;
                        }
                        else
                            debugMsg("PDDLAdpater","FAILED TO INJECT GOAL");
                    }
                }
              } // received
              usleep(10000); // avoid CPU overconsumption
            } // connected
        }
        debugMsg("PDDLAdapter","Finished TC PDDL thread.");
        return (void*) 0;
    }

    void PDDLAdapter::replacePlexilSpace(char* str)
    {
        for(unsigned int i=0; i<strlen(str); i++)
            if(str[i] == PLX_SPACE)
                str[i] = ' ';
    }

    bool PDDLAdapter::add_obstacle(char* ruta_mapa, int angleRobot, int objx,int objy, int bumper)
    {
        // Add obstacle in cost map
        int columnas=0; // Columnas del mapa
        int filas=0;    //Filas del mapa
        char line[200]; //Line
        int objx2,objy2; // Replica for add a second obstacle
        bool flag=false; //Flag for add a seccond obstacle
        objx2=objx;
        objy2=objy;
        FILE *file;
           
        if (angleRobot<0)
        {
            angleRobot=angleRobot+360; //Set range in [0º,360º]
        }
        
        if(DEBUG)
        {
            printf("\n\n**********ENTRO EN LA FUNCION PARA INRODUCIR OBSTACULO EN EL MAPA*************\n\n");
            debugMsg("ADD_OBSTACLE", "Ruta al mapa: "<<ruta_mapa);
            debugMsg("ADD_OBSTACLE", "Obstacle: "<<"x: "<<objx<<"y: "<<objy);
            debugMsg("ADD_OBSTACLE", "Bumper State: "<<bumper);
            debugMsg("ADD_OBSTACLE", "Orientation: "<<angleRobot);
        } 
        file=fopen(ruta_mapa,"r+"); //Open the file 
        if(file)
        {   
            //File open succesfull             
            columnas=strlen(fgets(line,200,file));  //columns length              
            //rows length
            rewind(file);
            while(!feof(file))
            {
                fread(line,sizeof(char),columnas,file);
                filas++;
            }
            filas--;

            // Add the obstacle in the next square where the robot stop. Next Square is obtained from the orientaion.
            if(angleRobot<22.5 || angleRobot>337.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objy++;
                        break;
                        case 2:
                        objx++;
                        break;
                        case 3:
                        objx++;
                        objy++;
                        flag=true;
                        objx2++;
                        break;
                        case 4:
                        objy--;
                        break;
                        case 6:
                        objx++;
                        objy--;
                        flag=true;
                        objx2++;
                        break;
                        case 7:
                        objx++;
                        break;
                        default:
                        objx++;
                        break;
                    }
                    
                }
                else if (angleRobot>=22.5 && angleRobot<67.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objx++;
                        break;
                        case 2:
                        objx++;
                        objy--;
                        break;
                        case 3:
                        objx++;
                        flag=true;
                        objx2++;
                        objy2--;
                        break;
                        case 4:
                        objy--;
                        break;
                        case 6:
                        objy--;
                        flag=true;
                        objy2--;
                        objx2--;
                        break;
                        case 7:
                        objx++;
                        objy--;
                        break;
                        default:
                        objx++;
                        objy--;
                    }

                }
                else if (angleRobot>=67.5 && angleRobot<112.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objx++;
                        break;
                        case 2:
                        objy--;
                        break;
                        case 3:
                        objx++;
                        objy--;
                        flag=true;
                        objy2--;
                        break;
                        case 4:
                        objx--;
                        break;
                        case 6:
                        objx--;
                        objy--;
                        flag=true;
                        objy2--;
                        break;
                        case 7:
                        objy--;
                        break;
                        default:
                        objy--;
                        break;
                    }

                }
                else if (angleRobot>=112.5 && angleRobot<157.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objy--;
                        break;
                        case 2:
                        objx--;
                        objy--;
                        break;
                        case 3:
                        objy--;
                        flag=true;
                        objx2--;
                        objy2--;
                        break;
                        case 4:
                        objx--;
                        break;
                        case 6:
                        objx--;
                        flag=true;
                        objx2--;
                        objy2--;
                        break;
                        case 7:
                        objx--;
                        objy--;
                        break;
                        default:
                        objx--;
                        objy--;
                        break;
                    }
                }
                else if (angleRobot>=157.5 && angleRobot<202.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objy--;
                        break;
                        case 2:
                        objx--;
                        break;
                        case 3:
                        objx--;
                        objy--;
                        flag=true;
                        objx2--;
                        break;
                        case 4:
                        objy++;
                        break;
                        case 6:
                        objx--;
                        objy++;
                        flag=true;
                        objx2--;
                        break;
                        case 7:
                        objx--;
                        break;
                        default:
                        objx--;
                        break;
                    }
                }
                else if (angleRobot>=202.5 && angleRobot<247.5)
                {
                     switch(bumper)
                    {
                        case 1:
                        objx--;
                        break;
                        case 2:
                        objx--;
                        objy++;
                        break;
                        case 3:
                        objx--;
                        flag=true;
                        objx2--;
                        objy2++;
                        break;
                        case 4:
                        objy++;
                        break;
                        case 6:
                        objy++;
                        flag=true;
                        objx2--;
                        objy2++;
                        break;
                        case 7:
                        objx--;
                        objy++;
                        break;
                    }

                }
                else if (angleRobot>=247.5 && angleRobot<292.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objx--;
                        break;
                        case 2:
                        objy++;
                        break;
                        case 3:
                        objx--;
                        objy++;
                        flag=true;
                        objy2++;
                        break;
                        case 4:
                        objx++;
                        break;
                        case 6:
                        objx++;
                        objy++;
                        flag=true;
                        objy2++;
                        break;
                        case 7:
                        objy++;
                        break;
                        default:
                        objy++;
                        break;
                    }
                    
                }
                else if (angleRobot>=292.5 && angleRobot<337.5)
                {
                    switch(bumper)
                    {
                        case 1:
                        objy++;
                        break;
                        case 2:
                        objx++;
                        objy++;
                        break;
                        case 3:
                        objy++;
                        flag=true;
                        objx2++;
                        objy2++;
                        break;
                        case 4:
                        objx++;
                        break;
                        case 6:
                        objx++;
                        flag=true;
                        objx2++;
                        objy2++;
                        break;
                        case 7:
                        objx++;
                        objy++;
                        break;
                        default:
                        objx++;
                        objy++;
                    }                  
                    
                }
            rewind(file); //set the pointer at start of the file.
            fseek(file,(columnas*sizeof(char)*(filas-objx-1))+(2*objy+1),1); //add obstacle in the x y position
            fputc('9',file);
            if (flag)
            {
                rewind(file); //set the pointer at start of the file.
                fseek(file,(columnas*sizeof(char)*(filas-objx2-1))+(2*objy2+1),1); //add obstacle in the x y position
                fputc('9',file);
            }
            fclose(file);// Close the file.
            if (DEBUG)
            {
                debugMsg("ADD_OBSTACLE", "Obstacle added in: "<<"x: "<<objx<<"y: "<<objy);
                if(flag)
                {
                    debugMsg("ADD_OBSTACLE", "Second Obstacle added in: "<<"x: "<<objx2<<"y: "<<objy2);
                }
            }
            return true;    
        }
        else
        {
            // Failed to open file.
            return false;
        }
    }
    void PDDLAdapter::executeCommand(const LabelStr& name, const std::list<double>& args, ExpressionId dest, ExpressionId ack)
    {
        // OGATE METRIC
        operationalTime += ogate.stopTimer(9)/1000;
        // START OGATE TIMER FOR METRIC
        ogatePlanner.startTimer(0);

        bool success = true;
        std::string nStr = name.toString();
        std::list<double>::const_iterator largs = args.begin();
        unsigned int nplanner;
        if(nStr != "create_planner")
	        nplanner = static_cast<int> (*largs);
        else
	        nplanner = 0;
        debugMsg("PDDLAdapter", "Command execution request ["<< nStr <<"] for planner #"<< nplanner);
        m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
        m_execInterface.notifyOfExternalEvent();
        
        if(nStr == "connect_planner_to_ogate")
        {
            LabelStr compName(*largs);
            LabelStr ogateHost(*(++largs));
            int ogatePort = static_cast<int>(*(++largs));
            if(ogatePort > 0)
            {
                debugMsg("PDDLAdapter", "Connecting to OGATE at "<<ogateHost.toString()<<":"<<ogatePort<<" as \""<<compName.toString()<<"\"");
                ogatePlanner.changeName(compName.toString());
                success = ogatePlanner.connect(ogateHost.toString(), ogatePort);
                if(success)
                {
                    if(ogatePlanner.registerControlInterface("0"))
                    {
                        debugMsg("PDDLAdapter", "Registered Control Interface");
                        // Start the client thread
                        tcreplan = false;
                        threadSpawn(ogateClientThread, (void*) this, ogClientThread);
                    }else
                        debugMsg("PDDLAdapter", "Failed to register Control Interface");
                    if(ogatePlanner.registerDataInterface("0"))
                    {   debugMsg("PDDLAdapter", "Registered Data Interface");
                    }else
                    {
                        success = false;
                        debugMsg("PDDLAdapter", "Failed to register Data Interface");
                    }
                    if(ogatePlanner.hasDataInterface())
                        sendTMstatusToOgate(&ogatePlanner, tick, false, std::string("MissionTimeline.mission_timeline"), std::string("Idle"), (char*)"numplanner", nplanner, NULL);
                }
                else
                    debugMsg("PDDLAdapter", "ERROR: failed to connect to OGATE");
            }
            m_execInterface.handleValueChange(dest,success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
        }

        else if(nStr == "putObstacle") //Add obstacle in the cost map
        {
            char *map_route;
            char *tempok;
            char final_route[500];
            char aux[5];
            char * angle;
            int angleRobot;
            int bumper;
            int objx,objy;
            bool ok;

            map_route=planificador[nplanner]->obtenerRutaMapaCostes(); //The path to the map with garbage characters
            strcpy(final_route,map_route);
            //Search the map route.
            tempok=strtok(final_route,(char*)" ");
            if(tempok == NULL)
                debugMsg("PDDLAdapter", "ERROR: failed to find route");
            tempok=strtok(NULL,(char*)" ");
            if(tempok == NULL)
                debugMsg("PDDLAdapter", "ERROR: failed to find route");
            strcpy(final_route,tempok); 

            LabelStr obstacleSquare(*largs);// Square Obstacle.
            LabelStr orientationRobot(*(++largs));//Robot Orientation
            bumper = static_cast<int> (*(++largs));// Down Bumper; 1->right 2->center 4->left.

            strcpy(aux,orientationRobot.c_str());
            angle=strtok(aux,(char*)"a");
            angleRobot=atoi(angle);// Orientation Robot in degree [-180º,180º]
            getXYposition(obstacleSquare, &objx, &objy,&ok);// x and y coordinates of the obstacle                        
            if (ok)
            {                
                success=add_obstacle(final_route,angleRobot,objx,objy,bumper);//Put the obstacle in the map.
            }
            else
            {
                debugMsg("PDDLAdapter", "ERROR: failed to add obstacle");
                success=false;
            }
        }
        else if(nStr == "delete_goals")
        {
            debugMsg("PDDLAdapter", "Delete goals from PDDL problem ");
            success=planificador[nplanner]->eliminar_goals();
            if (success)
            {
                debugMsg("PDDLAdapter", "Goals deleted correctly   "<<success);
            }
            else
            {
                debugMsg("PDDLAdapter", "ERROR: Failed to deleted goals"<<success);
            }
        }

        else if(nStr == "create_planner")
        {
            LabelStr ruta(*largs);
            debugMsg("PDDLAdapter", "Configuration file: "<< ruta.toString());
            planificador.push_back(new Planner((char*)ruta.c_str()));
            ultimaacc.push_back(new stpredicado);
            strcpy(ultimaacc[ultimaacc.size()-1]->nombre, (char*)"INIT");
            unsigned int num = planificador.size()-1;
            running++;
            debugMsg("PDDLAdapter", "New planner ID#: "<< num);
            m_execInterface.handleValueChange(dest,num);
        }

        else //Comprobar que numplanner esta dentro del rango
        if(nplanner>=0 && nplanner<planificador.size())
        {
         if(nStr == "get_plan")
         {
            /********CODIGO AÑADIDO POR DIEGO PARA BORRAR EN EL PROBLEMA LAS DISTANCIAS ENTRE CASILLAS*/
            if (planificador[nplanner]->eliminar_inits())
            {
                debugMsg("PDDLAdapter", "Distancias eliminadas correctamente");
            }
            else
            {
                debugMsg("PDDLAdapter", "ERROR al eliminar las distancias");
            }
            if (planificador[nplanner]->eliminar_objs())
            {
                debugMsg("PDDLAdapter", "Objs eliminadas correctamente");
            }
            else
            {
                debugMsg("PDDLAdapter", "ERROR al eliminar los Objs");
            }
            /******************************************************************************************/
            bool replan = static_cast<bool> (*(++largs));
            if(replan) 
            {   
                debugMsg("PDDLAdapter", "REPLANNING request in planner #"<<nplanner);
            }
            else 
                debugMsg("PDDLAdapter", "Planning request in planner #"<<nplanner);
            
            if(ogatePlanner.hasDataInterface())
            {
                sendTMstatusToOgate(&ogatePlanner, tick, false, std::string("MissionTimeline.mission_timeline"), std::string(replan?"Replanning":"Planning"), (char*)"numplanner", nplanner, NULL);
                sendMetricToOgate(&ogatePlanner, tick, std::string(DBTIME), 0, false);
            }
            // OGATE DELIBERATION TIME METRIC
            ogatePlanner.startTimer(0);
            int tickinit = tick;
            
            int retplan = planificador[nplanner]->planificar(replan);
            success = retplan==0;
            if(!success)
                debugMsg("PDDLAdapter","PDDL lib failure code: "<<retplan);
            tcreplan = !success;
            
            double deliberationtime = ogatePlanner.stopTimer(0)/1000;
            if(ogatePlanner.hasDataInterface())
            {
                // OGATE DELIBERATION TIME METRIC
                sendMetricToOgate(&ogatePlanner, ((tickinit+tick)/2), std::string(DBTIME), deliberationtime, true);
                sendTMstatusToOgate(&ogatePlanner, (tickinit!=tick?tick:tickinit+1), false, std::string("MissionTimeline.mission_timeline"), std::string("Idle"), (char*)"numplanner", nplanner, NULL);

                // Send plan to OGATE
                if(success)
                {
                    int timePlan = 0;
                    int numaccion = 1;
                    char *objs = new char[MAX_CAR];
		    char *token1;
		    char objeto1 [100];
		    char objeto2[100];
		    char *token2;
		    int y,x;
                    while(numaccion > 0)
                    {
                        numaccion = planificador[nplanner]->sig_accion(ultimaacc[nplanner]->nombre,objs,&ultimaacc[nplanner]->valor);
			if(strcmp(ultimaacc[nplanner]->nombre,"moveto")==0)
			{
				token1=strtok(objs," ");// First objet, delete it;
				token1=strtok(NULL," ");//Second Objet, 
				strcpy(objeto1,token1);
				token1=strtok(NULL," ");
				strcpy(objeto2,token1);
				objeto2[0]=' ';
				token2=strtok(objeto2,"_");
				y=atoi(token2);
				token2=strtok(NULL,"_");
				x=atoi(token2);
				
				sendTMstatusToOgate(&ogatePlanner, tick, true, std::string("MissionTimeline.mission_timeline"), std::string(ultimaacc[nplanner]->nombre), (char*)"numplanner", nplanner, (char*)"duration", (int)ultimaacc[nplanner]->valor, (char*)"start", timePlan, (char*)"end", (int)(timePlan+ultimaacc[nplanner]->valor), (char*)"Y",y, (char*)"X", x, NULL);
			}
                        	
			else
                        	sendTMstatusToOgate(&ogatePlanner, tick, true, std::string("MissionTimeline.mission_timeline"), std::string(ultimaacc[nplanner]->nombre), (char*)"numplanner", nplanner, (char*)"duration", (int)ultimaacc[nplanner]->valor, (char*)"start", timePlan, (char*)"end", (int)(timePlan+ultimaacc[nplanner]->valor), NULL);
                        timePlan += ultimaacc[nplanner]->valor;
                    }
                    planificador[nplanner]->reiniciarPlan();
                }
            }
            
            m_execInterface.handleValueChange(dest,success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
         }
         else if(nStr == "next_action")
         {
           if(tcreplan)
           {
               debugMsg("PDDLAdapter", "TC GOAL IMPLIES REPLAN!");
               LabelStr aux(REPLAN);
               m_execInterface.handleValueChange(dest, aux.getKey());
               success = true;
           }
           else
           {
            int numobj;
            char *objs = new char[MAX_CAR];
//            if(ultimaacc[nplanner]->objetos != NULL)
//                delete (char*)ultimaacc[nplanner]->objetos;
            planificador[nplanner]->sig_accion(ultimaacc[nplanner]->nombre,objs,&ultimaacc[nplanner]->valor);
            numobj=planificador[nplanner]->numero_objetos_proceso();
            ultimaacc[nplanner]->objetos = new char*[numobj];
            char *aux=strtok(objs,(char*)" ");
            int i=0;
            while(aux!=NULL)
            {
                ultimaacc[nplanner]->objetos[i]=new char[strlen(aux)];
                strcpy(ultimaacc[nplanner]->objetos[i],(char*)aux);
                aux=strtok(NULL," ");
                i++;
            }
    	    delete[] objs;
	        debugMsg("PDDLAdapter", "Next action is "<<ultimaacc[nplanner]->nombre<< " <> ["<<planificador[nplanner]->conector_accion()<<"]");
            LabelStr accion(planificador[nplanner]->conector_accion());
            if(accion.toString() == FINAL_PLAN)
                running--;

            m_execInterface.handleValueChange(dest,accion.getKey());
           }
         }
         else if(nStr == "action_object")
         {
            int nobj = static_cast<int> (*(++largs));
            if(nobj>0 && nobj<=planificador[nplanner]->numero_objetos_proceso())
            {
                LabelStr obj(ultimaacc[nplanner]->objetos[nobj-1]);
	            debugMsg("PDDLAdapter", "Object #"<<nobj<<" for last action is "<<obj.c_str());
                m_execInterface.handleValueChange(dest,obj.getKey());
            }
            else
            {
                success=false;
                LabelStr vacia;
                m_execInterface.handleValueChange(dest,vacia.getKey());
            }
         }
         else if(nStr == "action_duration")
         {
	        debugMsg("PDDLAdapter", "Duration for last action is "<<ultimaacc[nplanner]->valor);
	        m_execInterface.handleValueChange(dest,ultimaacc[nplanner]->valor);
         }
         else if(nStr == "insert_predicate")
         {
            LabelStr predicado(*(++largs));
            LabelStr objetos(*(++largs));
            bool meta = static_cast<bool> (*(++largs));
            char *cobjs = (char*)objetos.c_str();
            replacePlexilSpace(cobjs);
	        debugMsg("PDDLAdapter", "Insert "<<predicado.c_str()<<" "<<cobjs<<(meta?" as goal":""));
            success= planificador[nplanner]->insertar_pred((char*)predicado.c_str(), cobjs, meta) == 0;
            m_execInterface.handleValueChange(dest, success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
         }
         else if(nStr == "modify_predicate")
         {
            LabelStr predicado(*(++largs));
            LabelStr objetos(*(++largs));
            LabelStr nuevos(*(++largs));
            char *cobjs = (char*)objetos.c_str();
            char *cnews = (char*)nuevos.c_str();
            replacePlexilSpace(cobjs);
            replacePlexilSpace(cnews);
	        debugMsg("PDDLAdapter", "Replace ["<<cobjs<<"] with ["<<cnews<<"] on ("<<predicado.c_str()<<")");
            success= planificador[nplanner]->reemplazar_pred((char*)predicado.c_str(), cobjs, cnews) == 0;
            m_execInterface.handleValueChange(dest,success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
         }
         else if(nStr == "remove_predicate")
         {
            LabelStr predicado(*(++largs));
            LabelStr objetos(*(++largs));
	        debugMsg("PDDLAdapter", "Remove ("<<predicado.c_str()<<" "<<objetos.c_str()<<")");
            success= planificador[nplanner]->eliminar_predicado((char*)predicado.c_str(),(char*)objetos.c_str()) == 0;
            m_execInterface.handleValueChange(dest,success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
         }
         else if(nStr == "modify_function")
         {
            LabelStr predicado(*(++largs));
            LabelStr objetos(*(++largs));
            float valor = static_cast<float>(*(++largs));
	        debugMsg("PDDLAdapter", "Modify function value for ("<<predicado.c_str()<<" "<<objetos.c_str()<<"), new value: "<<valor);
            success= planificador[nplanner]->reemplazar_funcion((char*)predicado.c_str(),(char*)objetos.c_str(),valor) == 0;
            m_execInterface.handleValueChange(dest,success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
         }
         else if(nStr == "predicate_object")
         {
            LabelStr predicado(*(++largs));
            int nobj = static_cast<int> (*(++largs));
            char aux[31];
            strcpy(aux,(char*)predicado.c_str());
            success = planificador[nplanner]->objeto_pred((char*)aux,nobj);
            LabelStr objeto(aux);
	        debugMsg("PDDLAdapter", "Object #"<<nobj<<" for ("<<predicado.c_str()<<") is "<<objeto.c_str());
            m_execInterface.handleValueChange(dest,objeto);
         }
         else if(nStr == "function_value")
         {
            LabelStr funcion(*(++largs));
            LabelStr objetos(*(++largs));
            float valor;
            valor = planificador[nplanner]->valor_funcion((char*)funcion.c_str(),(char*)objetos.c_str());
	        debugMsg("PDDLAdapter", "Value for function ("<<funcion.c_str()<<" "<<objetos.c_str()<<") is "<<valor);
            m_execInterface.handleValueChange(dest,valor);
         }
        }else //PLANIFICADOR FUERA DE RANGO
            success=false;
    
        //STOP TIMER AND STORE METRIC
        // goal processing: action
        
        if(nStr == "next_action" || nStr == "action_object" || nStr == "action_duration")
            goalProcessingTime += ogatePlanner.stopTimer(0)/1000;
        // state processing: problem information
        if(nStr == "insert_predicate" || nStr == "modify_predicate" || nStr == "remove_predicate" ||
           nStr == "modify_function" || nStr == "function_value" || nStr == "predicate_object")
            stateProcessingTime += ogatePlanner.stopTimer(0)/1000;
        if(ogatePlanner.hasDataInterface())
        {
            bool send = sendMetricToOgate(&ogatePlanner, tick, std::string(GPTIME), goalProcessingTime, true) &&
                        sendMetricToOgate(&ogatePlanner, tick, std::string(SPTIME), stateProcessingTime, true) &&
                        sendMetricToOgate(&ogatePlanner, tick, std::string(EXTIME), operationalTime, true);
            if(send)
            {
                goalProcessingTime = 0;
                stateProcessingTime = 0;
                operationalTime = 0;
            }
        }
        if(success)
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SUCCESS());
        else
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_FAILED());
        m_execInterface.notifyOfExternalEvent();
        
        ogate.startTimer(9);
    }
    
// PLEXIL namespace
}

