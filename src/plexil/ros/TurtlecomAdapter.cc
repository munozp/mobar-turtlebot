#include "TurtlecomAdapter.hh"

// Register the TurtlecomAdapter
extern "C"
{
  void initTurtlecomAdapter()
  {
    REGISTER_ADAPTER(PLEXIL::TurtlecomAdapter, "TurtlecomAdapter");
  }
}

namespace PLEXIL
{
  // Constructor
  TurtlecomAdapter::TurtlecomAdapter(AdapterExecInterface& execInterface, const pugi::xml_node& xml)
    : InterfaceAdapter(execInterface, xml)
  {
    assertTrue(!xml.empty(), "XML config file not found in TurtlecomAdapter constructor");
    debugMsg("TurtlecomAdapter", " Using " << xml.attribute("AdapterType").value());
  }

  // Destructor
  TurtlecomAdapter::~TurtlecomAdapter()
  {
    debugMsg("TurtlecomAdapter", " Destructor called");
  }

  // Initialize
  bool TurtlecomAdapter::initialize()
  {
    debugMsg("TurtlecomAdapter::initialize", " called");
    m_execInterface.defaultRegisterAdapter(getId());
    debugMsg("TurtlecomAdapter::initialize", " done");
    return true;
  }

  // Start method
  bool TurtlecomAdapter::start()
  {
        tmout.tv_sec = 120;
        tmout.tv_usec = 0;
        tmout2.tv_sec = 20;
        tmout2.tv_usec = 0;
        posx = 0; posy = 0;
	    cumulativeDistance=0;
        head = 0;
        debugMsg("TurtlecomAdapter::start()", " called");
    return true;
  }

  // Stop method
  bool TurtlecomAdapter::stop()
  {
    debugMsg("TurtlecomAdapter::stop", " called");
    return true;
  }

  // Reset method
  bool TurtlecomAdapter::reset()
  {
    debugMsg("TurtlecomAdapter::reset", " called");
    return true;
  }

  // Shutdown method
  bool TurtlecomAdapter::shutdown()
  {
    debugMsg("TurtlecomAdapter::shutdown", " called");
    clnt_destroy (clnt);
    return true;
  }

  void TurtlecomAdapter::subscribe(const State& /* state */)
  {
    debugMsg("TurtlecomAdapter::subscribe", " called");
    debugMsg("ExternalInterface:udp", " subscribe called");
  }

  void TurtlecomAdapter::unsubscribe(const State& /* state */)
  {
    debugMsg("TurtlecomAdapter::unsubscribe", " called");
    debugMsg("ExternalInterface:udp", " unsubscribe called");
  }

/*
  Value TurtlecomAdapter::lookupNow(const State& state )
  {
    debugMsg("TurtlecomAdapter::lookupNow", " called");
    debugMsg("ExternalInterface:udp", " lookupNow called; returning UNKNOWN");
    return Expression::UNKNOWN();
  }
*/

float TurtlecomAdapter::moveBackxmeters(const LabelStr angle, const LabelStr square)
{
   int px,py,angleRobot;
   float distance;
   bool success;
   char * orientation;
   char aux[5]; 
   
    //Robot orientation in a number;
    strcpy(aux,angle.c_str());
    orientation=strtok(aux,(char*)"a");
    angleRobot=atoi(orientation); //range [-180,180º]
    if (angleRobot<0)
            {
                angleRobot=angleRobot+360; //Set range in [0º,360º]
            }
    //X Ycoordinates
    getXYposition(square,&px,&py,offsetx,offsety,&success);
    
    if(DEBUG)
    {
      debugMsg("TurtlecomAdapter:: MoveBackxMeters", "ROBOT ORIENTATION:"<<angleRobot<<" CELL POSITION: x="<<px<<" y="<<py);  
    }

    if(success)
    {
        // Square obtained correcctly, now check the orientation and obtain distance to move.
        if(angleRobot<22.5 || angleRobot>337.5)
        {
          //xpos --
          px--;
          distance=25;
        }
        else if (angleRobot>=22.5 && angleRobot<67.5)
        {
           // xpos++ ypos-- 
           px--;
           py++;
           distance=12;
        }
        else if (angleRobot>=67.5 && angleRobot<112.5)
        {         
           //ypos--
           py++;
           distance=25;
        }
        else if (angleRobot>=112.5 && angleRobot<157.5)
        {
            //xpos-- ypos--
            px++;
            py++;
            distance=12;
        }
        else if (angleRobot>=157.5 && angleRobot<202.5)
        {
            //xpos--
            px++;
            distance=25;
        }
        else if (angleRobot>=202.5 && angleRobot<247.5)
        {
            //xpos-- ypos++
            px++;
            py--;
            distance=12;
        }
        else if (angleRobot>=247.5 && angleRobot<292.5)
        {
            // ypos++
            py--;
            distance=25;
        }
        else if (angleRobot>=292.5 && angleRobot<337.5)
        {
            //xpos++ ypos++
            px--;
            py--;
            distance=12;
        }
        else
        {
            //ERROR
         debugMsg("TurtlecomAdapter::executeCommand", "ERROR:failure to obtain distance");
         return -1; 
        }  
        //px *= squareSize;
        //py *= squareSize;
        //distance=sqrt(pow(px-posx,2) + pow(py-posy,2));
        if(DEBUG)
        {
            debugMsg("TurtlecomAdapter:: MoveBackxMeters","NEW POSITION: x="<<px<<" y="<<py);
            debugMsg("TurtlecomAdapter:: MoveBackxMeters","CURRENT POSITION: x="<<posx<<" y="<<posy);
            debugMsg("TurtlecomAdapter:: MoveBackxMeters","DISTANCE TO MOVE BACK: "<<distance);  
        } 
        return distance;
        
    }

}
void TurtlecomAdapter::getXYposition(const LabelStr square, int *px, int *py, int offsetx, int offsety, bool *success)
{
    // Position x,y,z
    char aux[16];
    char* temptok;
    strcpy(aux, square.c_str());
    aux[0] = ' '; // Delete first 'C'
    temptok=strtok(aux, (char*)POS_DELIMITER); // First token, Y position
    if(temptok == NULL)
        *success = false; // Malformed square
    else
        //*px = atoi(temptok); LINEA CAMBIADA PARA LAS COORDENADAS!!!!!!!!!!!!!
	*py = atoi(temptok);
    // Next token
    temptok=strtok(NULL, (char*)POS_DELIMITER); // Second token, X position
    if(temptok == NULL)
        *success = false; // Malformed square
    else
        //*py = atoi(temptok);LINEA CAMBIADA PARA LAS COORDENADAS!!!!!!!!!!!!!
	*px = atoi(temptok);
     if(DEBUG)
     {
         debugMsg("TurtlecomAdapter::getXYPosition","Cadena que LLega desde PLEXIL: "<<aux);
         debugMsg("TurtlecomAdapter::getXYPosition","Posiciones X e Y:"<<"Cx= "<<*(px)<<" Cy= "<<*(py));
     }
    *success = true;
    // Quit inicial offset.
    *px-=offsetx;
    *py-=offsety;
    if(DEBUG)
    {
         debugMsg("TurtlecomAdapter::getXYPosition","Posiciones Corregidas: "<<"Cx="<<*(px)<<" Cy="<<*(py));  
    } 
}

  // Execute a Plexil Command
  void TurtlecomAdapter::executeCommand(const LabelStr& name, const std::list<double>&args,  ExpressionId dest, ExpressionId ack)
  {
        //OGATE METRIC
        double executionTime = ogate.stopTimer(9)/1000;
        if(ogate.hasDataInterface())
                sendMetricToOgate(&ogate, tick, std::string(EXTIME), executionTime, true);
        ogate.startTimer(0);
        double goalProcessingTime, stateProcessingTime;
        if (DEBUG)
        {
            debugMsg("TurtlecomAdapter::executeCommand", " " << name.toString());
        }
        std::list<double>::const_iterator largs = args.begin();
        bool retorno = false; //Si retorno =1 devuelve a plexil cadena de caracteres, y si es cero devuelve un entero
        bool retval = false;
        std::string nStr = name.toString();
        int * estado_sensor;
        char *checkbumpersemsorstate_1_arg;
        char *readbaterylevel_1_arg;
        char *checkchargin_1_arg;
        char *autodocking_1_arg;
        char *takepicture_1_arg;
        char *checkposition_1_arg;
        DATOS  datos_ROS;
        RESULTADO  *result;
        bool forward = false;
        bool rotate = false;
        float angrot;
        float dismov;
        int * chargerState;
        float * bateryLevel;
        int * autodockingState;
        int * takepictureState;
        bool result_movebackxmeters;
        int  *result_stop_command;
        bool restartDockServerState;
//*****************************************
        int newx, newy, newhead;
  
        if(nStr == "init_locomotion")
        {
            // Run rpc_client
            LabelStr ip(*largs);
            char host [20];
            strcpy(host, ip.c_str());
            if(ogate.hasDataInterface())
            {
                    sendTMstatusToOgate(&ogate, tick, false, std::string("Communication.communication"), std::string("Idle"), NULL);
                    sendTMstatusToOgate(&ogate, tick, false, std::string("Camera.camera"), std::string("Idle"), NULL);
            }
            clnt = clnt_create (host, NODO_COMUNICACION, NODO_COMUNICACIONVERS, "tcp");//Create RPC client
            clnt2 = clnt_create (host, NODO_COMUNICACION,NODO_COMUNICACIONVERS, "tcp");//Create  Second RPC client
            if (clnt == NULL )//|| clnt2==NULL) 
            {
                    debugMsg("TurtlecomAdapter","FAILED TO CONNECT ROS RPC SERVER.");
                    exit (1);
            }      
            clnt_control(clnt, CLSET_TIMEOUT, (char*)&tmout);
            clnt_control(clnt2, CLSET_TIMEOUT, (char*)&tmout2);            
            squareSize = static_cast<int> (*(++largs));// Set Square size            
	        offsetx = static_cast<int> (*(++largs)); //set squre offset x
	        offsety = static_cast<int> (*(++largs)); // Set suare offset y
	    
            if (DEBUG)
            {
                debugMsg("TurtlecomAdapter",": ROS RPC CLIENT SUCCESSFULL."<< "\n\r IP SERVER: "<<host);
                debugMsg("TurtlecomAdapter", nStr << ": Square size is " << squareSize << " cm");
                debugMsg("TurtlecomAdapter", nStr << ":Offset Cx:" << offsetx << " Offset Cy:" << offsety);
            }
            retval = true;
        }
        else if(nStr == "moveto")
        {
            LabelStr square(*largs);
            int distx, disty;
            bool correct;
            getXYposition(square, &newx, &newy,offsetx,offsety, &correct); //Get x and y position from string
            if(correct)
            {
                newx *= squareSize;
                newy *= squareSize;
                distx=newx-posx;
                disty=newy-posy;
                dismov = sqrt(pow(newx-posx,2) + pow(newy-posy,2));
                newhead=(acos(distx/dismov)*180/3.14159264);
                if (DEBUG)
                {
                    
                    debugMsg("TurtlecomAdapter:: Move_To", "Previous x: "<<posx<< "Previous y: "<<posy);
                    debugMsg("TurtlecomAdapter:: Move_To", "Next x: "<<newx<< "Next y: "<<newy); 
                    debugMsg("TurtlecomAdapter", "DEST IS CELL "<< square.c_str() << " = ("<< newx <<","<< newy << ") cm |  HEADING: " << newhead << "º");
                }
                if (distx>0 && disty <0)
                {
	                newhead=newhead;
                }
                else if (distx<0 && disty >0)
                {
	                newhead=-newhead;
                }
                else if (distx>0 && disty >0)
                {
		            newhead=-newhead;
                }
                else if (distx<0 && disty <0)
                {
		            newhead=newhead;
                }
                else if (distx==0)
                {
                    if (disty>0 )
                    {
			            newhead=-90;
                    }
                    else
                    {                               
			            newhead=90;
                    }
                }
                else if (disty==0)
                {
                    if (distx>0) 
                    {
                        newhead=0;
                    }
                    else
                    {
                        newhead=180;
                    }
                } 
                if(newhead != head)
                {
                    rotate = true;
                    angrot=newhead -head;
                    if (angrot <-180)
                        angrot=angrot+360;
                    if (angrot >180)
                        angrot=angrot-360;                       
                }
                if(newx != posx || newy != posy)
                {
                    forward = true;
                    dismov = sqrt(pow(newx-posx,2) + pow(newy-posy,2));
	                cumulativeDistance+=(double)(dismov/100.f);
                }
            }
            else
            {
                    retorno=false;
                    retval =false;
            }
            if (DEBUG)
            {
                debugMsg("TurtlecomAdapter:: Move_To", "Previous head: "<<head<< "New head: "<<newhead);
                debugMsg("TurtlecomAdapter:: Move_To", "Dismov: "<<dismov<< "AngRot: "<<angrot);
            }
        }//moveto
        else if (nStr == "moveBackxMeters")
        {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();
            if(ogate.hasDataInterface())
                sendTMstatusToOgate(&ogate, tick, false, std::string("RobotBase.robot_base"), std::string("Move Back"), NULL);	
	        goalProcessingTime = ogate.stopTimer(0)/1000;
	        if(ogate.hasDataInterface())
	            sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            LabelStr angle(*largs);
            LabelStr current_square(*(++largs));
            dismov=moveBackxmeters(angle,current_square);
            if (dismov==-1)
            {
                //Failed to obtain destance
                result_movebackxmeters=false;
                retorno=false;
                retval=false;
            }
            else
            {
                datos_ROS.vel_linear=-0.12;
                datos_ROS.vel_angular=0.0f;
                datos_ROS.avance=dismov/100.f;
                //datos_ROS.avance=21/100.f;
                result = locomotionforwardrqstandrcv_1(&datos_ROS, clnt);
                result_movebackxmeters=true;
                retorno=true;
                retval=true;
            }
        }
        else if (nStr == "bumpersensor")
        {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();

            goalProcessingTime = ogate.stopTimer(0)/1000;
            if(ogate.hasDataInterface())
	            sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            estado_sensor = checkbumpersemsorstate_1((void*)&checkbumpersemsorstate_1_arg, clnt);

            ogate.startTimer(0);
            retorno=false;
            retval =true;
        }
       else if (nStr == "readBateryLevel")
       {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();

	        goalProcessingTime = ogate.stopTimer(0)/1000;
	        if(ogate.hasDataInterface())
		        sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            bateryLevel=readbaterylevel_1((void*)&readbaterylevel_1_arg, clnt); //Call RPC_server.
	
	        ogate.startTimer(0);

            if(ogate.hasDataInterface())
                    sendMetricToOgate(&ogate, tick, std::string("Battery"), *(bateryLevel), false);
            if (DEBUG)
            {
            debugMsg("TurtlecomAdapter:: Read Batery Level", "Level: "<< *(bateryLevel)<< "%");
            }
            retorno=false;
            retval =true;
       }
       else if (nStr == "checkCharging")
       {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();

	        goalProcessingTime = ogate.stopTimer(0)/1000;
	        if(ogate.hasDataInterface())
		        sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);
	
            chargerState=checkchargin_1((void*)&checkchargin_1_arg, clnt); //Call RPC_server.

	        ogate.startTimer(0);
            if (DEBUG)
            {
            debugMsg("TurtlecomAdapter:: Batery State:", "State: "<< (*(chargerState)? "CHARGING":"DISCHARGING"));
            }
            retorno=false;
            retval =true;
       }
       else if (nStr == "autodocking")
       {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();

            if(ogate.hasDataInterface())
                sendTMstatusToOgate(&ogate, tick, false, std::string("RobotBase.robot_base"), std::string("Docking"), NULL);	
	        goalProcessingTime = ogate.stopTimer(0)/1000;
	        if(ogate.hasDataInterface())
		        sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);
	
            autodockingState= autodocking_1((void*)&autodocking_1_arg, clnt);	
std::cout<<"***************VOY A ASIGNAR -2 A LA VARIABLE AUTODOCKINSTATE: "<<*(autodockingState)<<std::endl;
           // *(autodockingState)=-2;/////////////////////////////////////////////////////
	        ogate.startTimer(0);
std::cout<<"HE PASADO START TIMER"<<std::endl;
            if(ogate.hasDataInterface())
		    sendTMstatusToOgate(&ogate, tick, false, std::string("RobotBase.robot_base"),  std::string("Dock"), (char*)"Y", (int)(round((-result->pos_actual_y*100.f)/float(squareSize))+offsety), (char*)"X", (int)(round((result->pos_actual_x*100.f)/float(squareSize))+offsetx), NULL);
std::cout<<"HE PASADO OGATE HS DATA INTERFACE"<<std::endl;
            if (DEBUG)
                std::cout<<"***********************AutodockingState: "<<*(autodockingState)<<std::endl;

            if(*(autodockingState)==-1)
            {
                //Error, server autodock Fail.
                if (DEBUG)
                  debugMsg("TurtlecomAdapter", "DOCK STATE: "<<"ERROR: Autodock Server Fails");                 
            }
            else if(*(autodockingState)==-2)
            {
                //Error, Docking drive Fail.
                if (DEBUG)
                  debugMsg("TurtlecomAdapter", "DOCK STATE: "<<"ERROR: Dock Drive Fails");                 
            }
            else if(*(autodockingState)==0)
            {
                //Error, Docking drive Fail.
                if (DEBUG)
                  debugMsg("TurtlecomAdapter", "DOCK STATE: "<<"Dock OK");                 
            }
            retorno=false;
            retval =true;
       }
       else if (nStr == "takePicture")
       {
            if (DEBUG)
            {
                debugMsg("TurtlecomAdapter:: TakePicture","I go to take the picture"<<endl);
            }
            printf("Estoy dentro de takePicture en plexil\n\n");
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();
            if(ogate.hasDataInterface())
                sendTMstatusToOgate(&ogate, tick, false, std::string("Camera.camera"), std::string("TakingPicture"), NULL);
            if(ogate.hasDataInterface())
                sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            takepictureState= takepicture_1((void*)&takepicture_1_arg, clnt);

            ogate.startTimer(0);

            if(ogate.hasDataInterface())
                sendTMstatusToOgate(&ogate, tick, false, std::string("Camera.camera"), std::string("Idle"), NULL);
            if (DEBUG)
            {
            debugMsg("TurtlecomAdapter", "PICTURE: "<< (*(takepictureState)? "PICTURE OK":"PICTURE FAIL"));
            }
            retorno=false;
            retval =true;
       }
       else if (nStr == "restartDockServer")
       {
            int  *result_restart_dock;
            char *restar_dock_arg;
            
            if (DEBUG)
            {
                debugMsg("TurtlecomAdapter", "Restart DOCK SERVER");
            }
    
            result_restart_dock=restartdockserver_1((void*)&restar_dock_arg, clnt);


	        if (result_restart_dock == (int *) NULL) 
            {
		        clnt_perror (clnt2, "Call Wake Up Server (RPC) Fails");
	        }
            if (*(result_restart_dock)==0)
            {
                 debugMsg("TurtlecomAdapter", "DOCK SERVER is awake");
                 restartDockServerState=true;
                 retorno=false;
                 retval =true;
            }
            else
            {
                debugMsg("TurtlecomAdapter","ERROR: Couldn`t wake up DOCK SERVER");
                restartDockServerState=false;
                retorno=false;
                retval =false;
            }

       }
       else if (nStr == "stopAction")
       {
            //This commnad checks the action to stop and calls the specific RPC function.

            LabelStr action_received(*largs);
            char action [20];
        	int  *result_10;
	        char *stopautodock_1_arg;

            strcpy(action, action_received.c_str());
            if (DEBUG)
            {
                debugMsg("TurtlecomAdapter", "STOP ACTION");
                //debugMsg("TurtlecomAdapter", "Action to stop: "<<action<<".");
            }            
            //if (strcmp(action,"dock")==0)
            //{
                //Stop Autodocking action.        
            	result_10 = stopautodock_1((void*)&stopautodock_1_arg, clnt2);
	            if (result_10 == (int *) NULL) 
                {
		            clnt_perror (clnt, "call failed");
	            }
                else
                {
                    retorno=false;
                    retval =true;
                }
            //}
            //else
            //{
                //Mandar mensaje de error porque el comando no se ha recibido correctamente.
            //}
       }
       else if (nStr == "checkPosition" || "checkAngle")
       {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();

            if(ogate.hasDataInterface())
	            sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            result = checkposition_1((void*)&checkposition_1_arg, clnt); //Call RPC server.

            ogate.startTimer(0);

            retorno=true;
            retval =true;

            if(ogate.hasDataInterface())
	            sendTMstatusToOgate(&ogate, tick, false, std::string("RobotBase.robot_base"), std::string("At"), (char*)"Y", (int)(-round((result->pos_actual_y*100.f)/float(squareSize))+offsety), (char*)"X", (int)(round((result->pos_actual_x*100.f)/float(squareSize))+offsetx), NULL);
       }

       if (nStr == "rotate" || rotate)
       {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();
            if(!rotate)
            {                   
                char aux[20];
                LabelStr angrot_received(*largs);
                strcpy(aux, angrot_received.c_str());
                aux[0] = ' ';
                angrot=double(atof(aux));
            }
            datos_ROS.vel_linear=0.0f;
            datos_ROS.vel_angular=0.4f;
            datos_ROS.angulo_rotacion=angrot;
	        if(ogate.hasDataInterface())
		        sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            result = locomotionrotaterqstandrcv_1(&datos_ROS, clnt); //Call RPC server.
	
	        ogate.startTimer(0);
	
            if (result== (RESULTADO *) NULL) {
                    clnt_perror (clnt, "call failed");
            }
            retorno=true;
            retval =true;
       }

      if (nStr == "forward" || forward || nStr=="moveBack")
      {
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
            m_execInterface.notifyOfExternalEvent();
            if(!forward)
            {
                dismov = static_cast<float> (*largs); 
            }
            if (nStr=="moveBack")
            {
                datos_ROS.vel_linear=-0.12;
                datos_ROS.vel_angular=0.0f;
                datos_ROS.avance=dismov/ 100.f;
            }
            else
            {       
                datos_ROS.vel_linear=0.12;
                datos_ROS.vel_angular=0.0f;
                datos_ROS.avance=dismov / 100.f;
            }
            
            if(ogate.hasDataInterface())
            sendTMstatusToOgate(&ogate, tick, false, std::string("RobotBase.robot_base"), std::string("GoingTo"), (char*)"Y", (int)round((newy)/float(squareSize))+offsety, (char*)"X", (int)round((newx)/float(squareSize))+offsetx, NULL);

            if(ogate.hasDataInterface())
                sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);

            result = locomotionforwardrqstandrcv_1(&datos_ROS, clnt);
            forward=false;//Añadido por diego porque nunca se pone a false.

            ogate.startTimer(0);

            if (result == (RESULTADO *) NULL) {
                clnt_perror (clnt, "call failed");
            }

            retorno=true;
            retval =true;
      }
    
      if(retorno==true)
      {
            posx = (int)(result->pos_actual_x*100.f);
            posy = -(int)(result->pos_actual_y*100.f);// Sign changed to work with the map
            head = (int)result->orientacion_actual;
            if (head>180)
            {
                head=head-360; //set head inrange [-180,180]
            }
            char position_return[20];
            int cx =int(round(posx /float(squareSize)))+offsetx;
            int cy =int(round(posy /float(squareSize)))+offsety;
            if ( nStr=="rotate" || nStr=="checkAngle")
            {
                sprintf(position_return,"a%d",head);
                if(DEBUG)
                    debugMsg("TurtlecomAdapter:: Return Orientation", "HEADING: "<<head<< "º");
            }
            else
            {       
            sprintf(position_return,"c%d_%d", cy,cx);

            if(ogate.hasDataInterface())
            {
                sendTMstatusToOgate(&ogate, tick, false, std::string("RobotBase.robot_base"), std::string("At"), (char*)"Y", cy, (char*)"X", cx, NULL);
                sendMetricToOgate(&ogate, tick,std::string("Distance"), cumulativeDistance, false);
            }
            if (DEBUG)
            {
                debugMsg("TurtlecomAdapter:: Return Position", "AT CELL: "<< position_return <<"C_"<< posx <<","<< posy <<" cm"); 
                debugMsg("TurtlecomAdapter:: Return Position", "Head: "<<head<< "º");
                debugMsg("TurtlecomAdapter:: Return Position", "Cumulative Distance: "<<cumulativeDistance<< "m");
            }
            }
            PLEXIL::LabelStr valor_devuelto(position_return);
            m_execInterface.handleValueChange(dest,valor_devuelto.getKey());
       }
       else if(nStr == "takePicture")
       {
            m_execInterface.handleValueChange(dest,*(takepictureState)? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
       }
       else if(nStr == "bumpersensor")
       {
            m_execInterface.handleValueChange(dest,*(estado_sensor));
       }
       else if(nStr == "readBateryLevel")
       {
            m_execInterface.handleValueChange(dest,*(bateryLevel));
       }
       else if(nStr == "autodocking")
       {
            m_execInterface.handleValueChange(dest,*(autodockingState));
       }  
       else if(nStr == "checkCharging")
       {
            m_execInterface.handleValueChange(dest,*(chargerState)? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
       }
       else if(nStr == "restartDockServer")
       {
            m_execInterface.handleValueChange(dest,restartDockServerState? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
       }
       else if (nStr == "stopAction")
       {
            m_execInterface.handleValueChange(dest,BooleanVariable::TRUE_VALUE());//CAMBIAR!!!!!!!!!!
       }
       if(retval)
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SUCCESS());
       else
            m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_FAILED());

       m_execInterface.notifyOfExternalEvent();  
       stateProcessingTime = ogate.stopTimer(0)/1000;
       if(ogate.hasDataInterface())
	        sendMetricToOgate(&ogate, tick, std::string(SPTIME), stateProcessingTime, true);
       //OGATE METRIC
       ogate.startTimer(9);

  }

}

